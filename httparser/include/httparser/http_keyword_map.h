/* C++ code produced by gperf version 3.0.1 */
/* Command-line: 'C:\\Workspace\\Tom Finet\\Main\\Source\\httcpp\\httparser\\gperf\\gperf.exe' -L C++ -t -K key 'C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf'  */
/* Computed positions: -k'1,6' */

#pragma once

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"

namespace httparser {
    enum method_t {
        GET, POST, OPTIONS, HEAD, PUT,
        DELETE, TRACE, CONNECT, EXTENSION
    };

    enum conn_t {
        KEEP_ALIVE, CLOSE
    };

    enum encoding_t {
        CHUNKED, IDENTITY, GZIP, COMPRESS, DEFLATE
    };

    enum content_type_t {
        HTML, JSON
    };
}
#line 21 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
struct http_kvp {
    const char* key;
    int value;
};

#define TOTAL_KEYWORDS 18
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 10
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 29
/* maximum key range = 27, duplicates = 0 */

class Perfect_Hash
{
private:
  static inline unsigned int hash (const char *str, unsigned int len);
public:
  static inline struct http_kvp *in_word_set (const char *str, unsigned int len);
};

inline unsigned int
Perfect_Hash::hash (const char *str, unsigned int len)
{
  static unsigned char asso_values[] =
    {
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 10,  0, 10,
      30, 10, 20, 30, 30, 30, 30, 30,  5,  5,
       0, 30, 30, 10, 10, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30,  0, 30,  0,
       5,  0, 30, 15,  5,  5,  0,  0, 30, 30,
      30, 30, 30, 30, 30, 30,  0, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      30, 30, 30, 30, 30, 30
    };
  int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[5]];
      /*FALLTHROUGH*/
      case 5:
      case 4:
      case 3:
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

inline struct http_kvp *
Perfect_Hash::in_word_set (const char *str, unsigned int len)
{
  static struct http_kvp wordlist[] =
    {
      {""}, {""}, {""},
#line 30 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"PUT", httparser::PUT},
#line 27 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"POST", httparser::POST},
#line 36 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"close", httparser::CLOSE},
      {""},
#line 37 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"chunked", httparser::CHUNKED},
#line 40 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"compress", httparser::COMPRESS},
#line 43 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"text/json", httparser::JSON},
#line 35 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"keep-alive", httparser::KEEP_ALIVE},
      {""},
#line 41 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"deflate", httparser::DEFLATE},
#line 26 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"GET", httparser::GET},
#line 42 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"text/html", httparser::HTML},
#line 32 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"TRACE", httparser::TRACE},
#line 31 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"DELETE", httparser::DELETE},
#line 28 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"OPTIONS", httparser::OPTIONS},
#line 38 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"identity", httparser::IDENTITY},
#line 39 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"gzip", httparser::GZIP},
      {""}, {""}, {""}, {""},
#line 29 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"HEAD", httparser::HEAD},
      {""}, {""},
#line 33 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"CONNECT", httparser::CONNECT},
      {""},
#line 34 "C:/Workspace/Tom Finet/Main/Source/httcpp/httparser/gperf/http_keyword_hash.gperf"
      {"EXTENSION", httparser::EXTENSION}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          const char *s = wordlist[key].key;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
