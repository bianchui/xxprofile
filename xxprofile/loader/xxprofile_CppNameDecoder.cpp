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
    token._name = false;
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
            } else if (ch == '[') {
                if (_current[1] == ']') {
                    _current += 2;
                    break;
                }
            } else if (ch == '(') {
                if (_current[1] == ')') {
                    _current += 2;
                    break;
                }
            } else if (ch == '+') {
                if (_current[1] == '+') {
                    _current += 2;
                    break;
                }
                if (_current[1] == '=') {
                    _current += 2;
                    break;
                }
            } else if (ch == '-') {
                if (_current[1] == '-') {
                    _current += 2;
                    break;
                }
                if (_current[1] == '=') {
                    _current += 2;
                    break;
                }
                if (_current[1] == '>') {
                    if (_current[2] == '*') {
                        _current += 3;
                        break;
                    }
                    _current += 2;
                    break;
                }
            } else if (ch == '*') {
                if (_current[1] == '=') {
                    _current += 2;
                    break;
                }
            } else if (ch == '/') {
                if (_current[1] == '=') {
                    _current += 2;
                    break;
                }
            } else if (ch == '%') {
                if (_current[1] == '=') {
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
    token._name = isName;
}

void CppNameDecoder::TokenParser::parseNext(Token& token) {
    _parseNext(token);
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
            if (token._name) {
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
        printf("  '%.*s'\n", (int)token.length, token.begin);
        builder._tokens.push_back(token);
    } while (true);

    builder.build();
}

XX_NAMESPACE_END(xxprofile);
