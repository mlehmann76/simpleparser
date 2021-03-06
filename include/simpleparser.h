#ifndef __SIMPLEPARSER_H_
#define __SIMPLEPARSER_H_

#include <ctype.h>
#include <functional>
#include <string_view>

namespace simpleparser {
namespace detail {
/**
 *
 */
template <typename Tchar, Tchar... str> class tokenizer {
public:
  using SType = std::basic_string_view<Tchar>;
  using ret_type = std::pair<SType, Tchar>;

  constexpr tokenizer(SType _p)
      : m_p(std::move(_p)), m_delimiter{str..., '\0'}, m_start(0), m_end(0) {}

  constexpr ret_type token() {
    ret_type temp;
    m_end = m_p.find_first_of(m_delimiter, m_start);
    if ((m_end != SType::npos) && (m_end > m_start)) {
      temp = {m_p.substr(m_start, m_end - m_start), m_p[m_end]};
      m_start = m_end + 1;
    } else {
      temp = {m_p.substr(m_start, m_p.length() - m_start), '\0'};
      m_start = m_p.length();
    }
    return temp;
  }

private:
  SType m_p;
  const Tchar m_delimiter[sizeof...(str) + 1];
  typename SType::size_type m_start, m_end;
};
} // namespace detail

/**
 *
 */
template <typename Tchar, int Size> class KeywordPattern {
  using num_type = int;
  using stype = std::basic_string_view<Tchar>;
  using size_type = typename std::basic_string_view<Tchar>::size_type;
  using part_type = std::pair<bool, num_type>;
  using tokenizer_type = detail::tokenizer<char, ':', ' ', '?'>;

public:
  struct result_type {
    stype rest;
    char sym;
    bool equal;
    int num;
  };

  enum error_type { MATCH_ERR, MATCH_OK };

public:
  //
  constexpr KeywordPattern(const stype &_k) : m_key(_k) {}
  //
  constexpr error_type match(const stype &_m) {
    tokenizer_type _m_token = {_m};
    tokenizer_type _key_token = {m_key};
    error_type ret = MATCH_ERR;
    for (m_last_index = 0; m_last_index < Size; m_last_index++) {
      // split token
      m_return[m_last_index] = {"", '\0', false, 0};
      auto _m_part = _m_token.token();
      auto _k_part = _key_token.token();
      // std::cout << _m_part.first << " - " << _k_part.first << std::endl;
      // test for number
      int withNumber = hasNumber(_k_part.first);
      // test for equality
      part_type _partRet = matchToken(_k_part.first, _m_part.first);
      m_return[m_last_index].num = _partRet.second;
      m_return[m_last_index].sym = _m_part.second;

      if (!_partRet.first) {
        break;
      }

      if (_m_part.second == '\0') {
        m_return[m_last_index].rest = _m_part.first;
        m_return[m_last_index].equal = true;
        ret = MATCH_OK;
        break;
      }
    }
    return ret;
  }

  constexpr result_type result(size_t index) const {
    return index < Size ? m_return[index] : result_type{"", '\0', false, 0};
  }

  constexpr int getSize() const { return m_last_index; }

private:
  // remove # token from keyword if used
  constexpr int hasNumber(stype &_part) const {
    int ret = 0;
    while (_part.find_first_of('#') != stype::npos) {
      _part.remove_suffix(1);
      ++ret;
    }
    return ret;
  }

  constexpr static part_type matchToken(const stype &_k, const stype &_m) {
    part_type ret = {true, 1}; // default is index 1
    bool _short = true;        // only short form needed

    for (auto _k_it = _k.begin(), _k_end = _k.end(), _m_it = _m.begin(),
              _m_end = _m.end();
         !((_k_it == _k_end) && (_m_it == _m_end)); _k_it++, _m_it++) {

      if ((_k_it == _k_end) && (_m_it != _m_end)) {
        if (isdigit(*_m_it)) {
          // handle numeric input
          ret.second = toInt(*_m_it);
          break;
        } else {
          // input longer than key
          ret.first = false;
          break;
        }
      }

      if ((_k_it != _k_end) && (_m_it == _m_end)) {
        ret.first = _short;
        break;
      }

      if (tolower(*(_k_it)) != tolower(*(_m_it))) {
        // could be digit in short form
        if (_short && !isupper(*(_k_it)) && isdigit(*_m_it)) {
          // handle numeric input
          ret.second = toInt(*_m_it);
          break;
        } else {
          ret.first = false;
          break;
        }
      }

      if (!(_k_it == _k_end) && !isupper(*(_k_it))) {
        _short = false; // full form needed
      }
    }

    return ret;
  }

  static int toInt(char c) { return c - '0'; }

  stype m_key;
  std::array<result_type, Size> m_return;
  int m_last_index;
};
/**
 *
 */
template <typename TChar, int Size> class KeywordPatternLink {
public:
  using StringType = std::basic_string_view<TChar>;
  using getter_type = std::function<StringType()>;
  using setter_type = std::function<void(StringType)>;
  using keyword_type = KeywordPattern<TChar, Size>;

  constexpr KeywordPatternLink(StringType &&pattern, getter_type &&getter)
      : KeywordPatternLink(pattern, getter, {}) {}

  constexpr KeywordPatternLink(StringType &&pattern, setter_type &&setter)
      : KeywordPatternLink(pattern, {}, setter) {}

  constexpr KeywordPatternLink(StringType &&pattern, getter_type &&getter,
                               setter_type &&setter)
      : m_key(pattern), m_setter{setter}, m_getter{getter} {}

  constexpr void match(StringType s) {
    auto ret = m_key.match(s);
    if (ret == keyword_type::MATCH_OK) {
      int index = m_key.getSize() > 0 ? m_key.getSize() - 1 : 0;

      if (m_key.result(index).sym == '?' && m_getter) {
        m_getter();
      }
      if (m_key.result(index).sym == ' ' && m_setter) {
        m_setter(StringType{m_key.result(index + 1).rest});
      }
    }
  }

private:
  keyword_type m_key;
  setter_type m_setter;
  getter_type m_getter;
};

} // namespace simpleparser

#endif // !__SIMPLEPARSER_H_