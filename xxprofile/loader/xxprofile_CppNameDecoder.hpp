// Copyright 2018 bianchui. All rights reserved.
#ifndef xxprofile_loader_xxprofile_CppNameDecoder_hpp
#define xxprofile_loader_xxprofile_CppNameDecoder_hpp
#include "../src/xxprofile_macros.hpp"
#include <stdint.h>
#include <ctype.h>
#include <string>
#include <vector>

XX_NAMESPACE_BEGIN(xxprofile);

struct CppNameDecoder {
    struct Token {
        struct Type {
            enum Enum {
                Unknown,
                Name,
                Operator,
                FixedOperator,
            };
        };

        const char* begin;
        size_t length;
        size_t column;
        Type::Enum _type;

        bool is(const char* str) const {
            return (strncmp(begin, str, length) == 0) && str[length] == 0;
        }
        bool is(char ch) const {
            return length == 1 && begin[0] == ch;
        }
        bool isName() const {
            return _type == Type::Name;
        }
        bool isOperator() const {
            return _type == Type::Operator;
        }
        bool isFixedOperator() const {
            return _type == Type::FixedOperator;
        }
    };

    struct TokenParser {
        explicit TokenParser(const char* data);
        void parseNext(Token& token);

    private:
        void _parseNext(Token& token);
        void _putBackToken(Token& token);

    private:
        const char* _data;
        const char* _current;
    };

    struct NameTree {
        std::string name;
        std::vector<NameTree> children;
    };

    CppNameDecoder(const char* name);
};

XX_NAMESPACE_END(xxprofile);

#endif//xxprofile_loader_xxprofile_CppNameDecoder_hpp
