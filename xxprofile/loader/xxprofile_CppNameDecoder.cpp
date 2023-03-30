// Copyright 2018 bianchui. All rights reserved.
#include "xxprofile_CppNameDecoder.hpp"
#include <stdio.h>

XX_NAMESPACE_BEGIN(xxprofile);

CppNameDecoder::TokenParser::TokenParser(const char* data) : _data(data), _current(data) {
}

static bool isFirstSymbolChar(char ch) {
    return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == '_') || (ch == '$');
}

static bool isSecondSymbolChar(char ch) {
    return isFirstSymbolChar(ch) || (ch >= '0' && ch <= '9');
}

void CppNameDecoder::TokenParser::_parseNextSimple(Token& token) {
    token.begin = _current;
    token.column = _current - _data + 1;
    token.length = 0;
    token._type = Token::Type::Unknown;
    if (!*_current) {
        return;
    }

    bool isName = false;
    while (true) {
        const char ch = *_current;
        if (ch == 0) {
            break;
        }
        if (ch == ' ') {
            if (_current == token.begin) {
                ++_current;
            }
            break;
        }
        if (_current == token.begin) {
            if (isFirstSymbolChar(ch)) {
                ++_current;
                isName = true;
                continue;
            } else if (ch == '~') {
                if (isFirstSymbolChar(_current[1])) {
                    _current += 2;
                    isName = true;
                    continue;
                } else {
                    ++_current;
                    break;
                }
            } else if (ch == ':') {
                if (_current[1] == ':') {
                    _current += 2;
                    break;
                }
            }
        } else {
            if (isName) {
                if (isSecondSymbolChar(ch)) {
                    ++_current;
                    continue;
                }
                break;
            } else {
                break;
            }
        }
        ++_current;
        break;
    }
    token.length = _current - token.begin;
    if (isName) {
        token._type = Token::Type::Name;
    }
}

void CppNameDecoder::TokenParser::_putBackToken(Token& token) {
    assert(token.begin + token.length == _current);
    _current = token.begin;
}

#if 0
// https://en.cppreference.com/w/cpp/language/expressions#Operators
// https://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B
static const char* const kOperators[] = {
    // Arithmetic operators
    "=", // R& K::operator =(S b);
    "+", // R K::operator +(S b) const; R K::operator +() const;
    "-", // R K::operator -(S b) const; R K::operator -() const;
    "*", // R K::operator *(S b) const;
    "/", // R K::operator /(S b) const;
    "%", // R K::operator %(S b) const;
    "++", // R& K::operator ++(); R K::operator ++(int);
    "--", // R& K::operator --(); R K::operator --(int);

    // Comparison operators/relational operators
    "==", // bool K::operator ==(S const& b) const;
    "!=", // bool K::operator !=(S const& b) const;
    "<", // bool K::operator <(S const& b) const;
    ">", // bool K::operator >(S const& b) const;
    "<=", // bool K::operator <=(S const& b) const;
    ">=", // bool K::operator >=(S const& b) const;

    // Logical operators
    "!", // bool K::operator !() const;
    "&&", // bool K::operator &&(S b) const;
    "||", // bool K::operator ||(S b) const;

    // Bitwise operators
    "~", // R K::operator ~() const;
    "&", // R K::operator &(S b) const;
    "|", // R K::operator |(S b) const;
    "^", // R K::operator ^(S b) const;
    "<<", // R K::operator <<(S b) const;
    ">>", // R K::operator >>(S b) const;

    // Compound assignment operators
    "+=", // R& K::operator +=(S b);
    "-=", // R& K::operator -=(S b);
    "*=", // R& K::operator *=(S b);
    "/=", // R& K::operator /=(S b);
    "%=", // R& K::operator %=(S b);
    "&=", // R& K::operator &=(S b);
    "|=", // R& K::operator |=(S b);
    "^=", // R& K::operator ^=(S b);
    "<<=", // R& K::operator <<=(S b);
    ">>=", // R& K::operator >>=(S b);

    // Member and pointer operators
    "[]", // R& K::operator [](S b) const;
    "*", // R& K::operator *();
    "&", // R* K::operator &();
    "->", // R* K::operator ->();
    "->*", // R& K::operator ->*(S b);

    // Other operators
    "()", // R K::operator ()(S a, T b, ...);
};
#endif//0

void CppNameDecoder::TokenParser::_parseNextWithFixedOperator(Token& token) {
    _parseNextSimple(token);
    if (token.length == 0) {
        return;
    }

    if (token.isName()) {
        // try to fix operators, make operator a full name
        if (token.is("operator")) {
            token._type = Token::Type::Operator;
            Token tk;
            _parseNextSimple(tk);
            if (tk.length == 0) {
                return;
            }
            if (tk.length == 1) {
                bool isFixedOperator = true;
                const char opCh = tk.begin[0];
                switch (opCh) {
                    case '=': // =, ==
                    case '*': // *, *=
                    case '/': // /, /=
                    case '%': // %, %=
                    case '!': // !, !=
                    case '^': // ^, ^=
                        token.length += tk.length;
                        _parseNextSimple(tk);
                        if (tk.is('=')) {
                            token.length += tk.length;
                        } else {
                            _putBackToken(tk);
                        }
                        break;

                    case '+': // +, ++, +=
                        token.length += tk.length;
                        _parseNextSimple(tk);
                        if (tk.is('+') || tk.is('=')) {
                            token.length += tk.length;
                        }  else {
                            _putBackToken(tk);
                        }
                        break;

                    case '-': // -, --, -=, ->, ->*
                        token.length += tk.length;
                        _parseNextSimple(tk);
                        if (tk.is('-') || tk.is('=')) {
                            token.length += tk.length;
                        } else if (tk.is('>')) {
                            token.length += tk.length;
                            _parseNextSimple(tk);
                            if (tk.is('*')) {
                                token.length += tk.length;
                            }  else {
                                _putBackToken(tk);
                            }
                        }  else {
                            _putBackToken(tk);
                        }
                        break;

                    case '<': // <, <=, <<, <<=
                    case '>': // >, >=, >>, >>=
                        token.length += tk.length;
                        _parseNextSimple(tk);
                        if (tk.is('=')) {
                            token.length += tk.length;
                        } else if (tk.is(opCh)) {
                            token.length += tk.length;
                            _parseNextSimple(tk);
                            if (tk.is('=')) {
                                token.length += tk.length;
                            }  else {
                                _putBackToken(tk);
                            }
                        }  else {
                            _putBackToken(tk);
                        }
                        break;

                    case '~': // ~
                        token.length += tk.length;
                        break;

                    case '&': // &, &&, &=
                    case '|': // |, ||, |=
                        token.length += tk.length;
                        _parseNextSimple(tk);
                        if (tk.is('=') || tk.is(opCh)) {
                            token.length += tk.length;
                        }  else {
                            _putBackToken(tk);
                        }
                        break;

                    case '[': // []
                        token.length += tk.length;
                        _parseNextSimple(tk);
                        if (tk.is(']')) {
                            token.length += tk.length;
                        } else {
                            _putBackToken(tk);
                        }
                        break;

                    case '(': // ()
                        token.length += tk.length;
                        _parseNextSimple(tk);
                        if (tk.is(')')) {
                            token.length += tk.length;
                        } else {
                            _putBackToken(tk);
                        }
                        break;

                    default:
                        isFixedOperator = false;
                        _putBackToken(tk);
                        break;
                }
                if (isFixedOperator) {
                    token._type = Token::Type::FixedOperator;
                }
            } else {
                _putBackToken(tk);
            }
        }
    }
    
    assert(token.begin + token.length == _current);
}

void CppNameDecoder::TokenParser::parseNext(Token& token) {
    _parseNextWithFixedOperator(token);
    if (token.length == 0) {
        return;
    }

    if (token.isName() || token.is("::")) {
        if (token.is("::")) {
            _putBackToken(token);
            token.length = 0;
        }
        while (true) {
            Token tk;
            _parseNextWithFixedOperator(tk);
            if (tk.is("::")) {
                token.length += tk.length;
                _parseNextWithFixedOperator(tk);
                if (tk.isName() || tk.isFixedOperator()) {
                    token.length += tk.length;
                } else {
                    _putBackToken(tk);
                    break;
                }
            } else {
                _putBackToken(tk);
                break;
            }
        }
    } else {

    }

    assert(token.begin + token.length == _current);
}

CppNameDecoder::CppNameDecoder(const char* name) {
    TokenParser parser(name);
    printf("%s\n", name);
    Token token;
    struct NameBuilder {
        std::vector<Token>& _tokens;
        NameTree& _root;

        NameBuilder(std::vector<Token>& tokens, NameTree& root) : _tokens(tokens), _root(root) {
        }

        void build() {
            size_t curIndex = 0;
            build(curIndex, _root);
        }

        void build(size_t& curIndex, NameTree& node) {
            while (curIndex < _tokens.size()) {
                const Token& token = _tokens[curIndex];
                node.children.resize(node.children.size() + 1);
                auto& child = node.children.back();
                child.token = token;
                ++curIndex;
                if (token.is('<')) {
                    build(curIndex, child);
                } else if (token.is('>')) {
                    return;
                } else if (token.is('(')) {
                    build(curIndex, child);
                } else if (token.is(')')) {
                    return;
                } else if (token.is('[')) {
                    build(curIndex, child);
                } else if (token.is(']')) {
                    return;
                } else {

                }
            }
        }
    };

    NameBuilder builder(_tokens, _root);

    do {
        parser.parseNext(token);
        if (!token.length) {
            break;
        }
        //printf("  '%.*s'\n", (int)token.length, token.begin);
        builder._tokens.push_back(token);
    } while (true);

    builder.build();
    builder._root.dump();
    shared::StrBuf buf;
    builder._root.build(buf);
    assert(strcmp(buf.c_str(), name) == 0);
}

XX_NAMESPACE_END(xxprofile);
