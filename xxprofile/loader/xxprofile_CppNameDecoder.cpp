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

void CppNameDecoder::TokenParser::_parseNext(Token& token) {
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

// https://en.cppreference.com/w/cpp/language/expressions#Operators
// https://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B
static const char* const kOperators[] = {
    // Arithmetic operators
    "=", // R& K::operator =(S b);
    "+", // R K::operator +(S b) const; R K::operator +() const;
    "-", // R K::operator -(S b) const; R K::operator -() const;
    "*", // R K::operator *(S b) const; R& K::operator *();
    "/", // R K::operator /(S b) const;
    "%", // R K::operator %(S b) const;
    "++", // R& K::operator ++(); R K::operator ++(int);
    "--", // R& K::operator --(); R K::operator --(int);

    // Comparison operators/relational operators
    "==", // bool K::operator ==(S const& b) const;
    "!=", // bool K::operator !=(S const& b) const;
    "<", // bool K::operator <(S const& b) const;
    ">", // bool K::operator >(S const& b) const;
    ">=", // bool K::operator >=(S const& b) const;
    "<=", // bool K::operator <=(S const& b) const;

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
    // R& K::operator *();
    "&", // R* K::operator &();
    "->", // R* K::operator ->();
    "->*", // R& K::operator ->*(S b);

    // Other operators
    "()", // R K::operator ()(S a, T b, ...);

};

void CppNameDecoder::TokenParser::parseNext(Token& token) {
    _parseNext(token);
    if (token.length == 0) {
        return;
    }

    if (token.isName()) {
        // try to fix operators, make operator a full name
        if (token.is("operator")) {
            Token tk;
            _parseNext(tk);
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
                        _parseNext(tk);
                        if (tk.is('=')) {
                             token.length += tk.length;
                        } else {
                            _putBackToken(tk);
                        }
                        break;

                    case '+': // +, ++, +=
                        token.length += tk.length;
                        _parseNext(tk);
                        if (tk.is('+') || tk.is('=')) {
                            token.length += tk.length;
                        }  else {
                            _putBackToken(tk);
                        }
                        break;

                    case '-': // -, --, -=, ->, ->*
                        token.length += tk.length;
                        _parseNext(tk);
                        if (tk.is('-') || tk.is('=')) {
                            token.length += tk.length;
                        } else if (tk.is('>')) {
                            token.length += tk.length;
                            _parseNext(tk);
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
                        _parseNext(tk);
                        if (tk.is('=')) {
                            token.length += tk.length;
                        } else if (tk.is(opCh)) {
                            token.length += tk.length;
                            _parseNext(tk);
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
                        _parseNext(tk);
                        break;

                    case '&': // &, &&, &=
                    case '|': // |, ||, |=
                        token.length += tk.length;
                        _parseNext(tk);
                        if (tk.is('=') || tk.is(opCh)) {
                            token.length += tk.length;
                        }  else {
                            _putBackToken(tk);
                        }
                        break;

                    case '[': // []
                        token.length += tk.length;
                        _parseNext(tk);
                        if (tk.is(']')) {
                            token.length += tk.length;
                        } else {
                            _putBackToken(tk);
                        }
                        break;

                    case '(': // ()
                        token.length += tk.length;
                        _parseNext(tk);
                        if (tk.is(')')) {
                            token.length += tk.length;
                        } else {
                            _putBackToken(tk);
                        }
                        break;

                    default:
                        isFixedOperator = false;
                        break;
                }
                if (isFixedOperator) {
                    token._type = Token::Type::FixedOperator;
                }
            }

        } else {
            
        }
    } else {

    }
}

CppNameDecoder::CppNameDecoder(const char* name) {
    TokenParser parser(name);
    printf("%s\n", name);
    Token token;
    struct NameBuilder {
        std::vector<Token> _tokens;
        NameTree _root;

        NameBuilder() {
        }

        void build() {
            build(0, _root);
        }

        void build(size_t curIndex, NameTree& node) {
            const Token& token = _tokens[curIndex];
            if (token.isName()) {
                if (curIndex + 1 < _tokens.size()) {
                    if (_tokens[curIndex + 1].is("::")) {

                    }
                }
            }
        }
    };

    NameBuilder builder;

    do {
        parser.parseNext(token);
        if (!token.length) {
            break;
        }
        //printf("  '%.*s'\n", (int)token.length, token.begin);
        builder._tokens.push_back(token);
    } while (true);

    builder.build();
}

XX_NAMESPACE_END(xxprofile);
