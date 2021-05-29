#ifndef __SIMPLEPARSER_H_
#define __SIMPLEPARSER_H_

#include <functional>
#include <string_view>

/**
 *
 */
template <typename SType,typename Tchar, Tchar... str> class tokenizer {
public:
  using ret_type = std::pair<SType, char>;

  tokenizer(SType _p)
      : m_p(std::move(_p)), m_delimiter{str..., '\0'}, m_start(0), m_end(0) {}

  ret_type part() {
    ret_type temp = {m_p.substr(m_start, m_p.length() - m_start), '\0'};
    m_end = m_p.find_first_of(m_delimiter, m_start);
    if ((m_end != SType::npos) && (m_end > m_start)) {
      temp = {m_p.substr(m_start, m_end - m_start), m_p[m_end]};
      m_start = m_end + 1;
    } else {
      m_start = m_p.length();
    }
    return temp;
  }

private:
  std::string_view m_p;
  const Tchar m_delimiter[sizeof...(str) + 1];
  std::string_view::size_type m_start, m_end;
};

/**
 *
 */
template <typename stype, int Size> class KeywordPattern {
  using num_type = int;
  using size_type = typename stype::size_type;
  using part_type = std::pair<bool, num_type>;
  using tokenizer_type = tokenizer<stype, char, ':', ' ', '?'>;

public:
  struct result_type {
    stype rest;
    char sym;
    bool equal;
    int num;
  };

  enum error_type {
    MATCH_ERR, MATCH_OK
  };

public:
  //
  KeywordPattern(const stype &_k) : m_key(_k) {}
  //
  error_type match(const stype &_m) {
    tokenizer_type _m_token = {_m};
    tokenizer_type _key_token = {m_key};
    error_type ret = MATCH_ERR;
    for (m_last_index = 0; m_last_index < Size; m_last_index++) {
      // split part
      m_return[m_last_index] = {"", '\0', false, 0};
      auto _m_part = _m_token.part();
      auto _k_part = _key_token.part();
      // std::cout << _m_part.first << " - " << _k_part.first << std::endl;
      // test for number
      int withNumber = hasNumber(_k_part.first);
      // test for equality
      part_type _partRet = _matchPart(_k_part.first, _m_part.first);
      // std::cout << "equal " << isEqual << std::endl;
      if (withNumber)
        m_return[m_last_index].num = _partRet.second;

      if (_m_part.second) {
        // save for later use
        m_return[m_last_index].sym = _m_part.second;
      }

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

  result_type result(size_t index) const {
    return index < Size ? m_return[index]
                        : result_type{"", '\0', false, 0};
  }

  int getSize() const { return m_last_index; }

private:
  // remove # part from keyword if used
  int hasNumber(stype &_part) const {
    int ret = 0;
    while (_part.find_first_of('#') != stype::npos) {
      _part.remove_suffix(1);
      ++ret;
    }
    return ret;
  }

  static part_type _matchPart(const stype &_k, const stype &_m) {
    part_type ret = {true, 1}; // default is index 1
    bool _short = true;        // only short form needed

    for (auto _k_it = _k.begin(), _k_end = _k.end(), _m_it = _m.begin(),
              _m_end = _m.end();
         ; _k_it++, _m_it++) {

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

      if ((_k_it == _k_end) && (_m_it == _m_end)) {
        break;
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
template <typename StringType, int Size>
class KeywordPatternLink {
public:
  using getter_type = std::function<StringType()>;
  using setter_type = std::function<void(StringType)>;
  using keyword_type = KeywordPattern<StringType, Size>;

  KeywordPatternLink(StringType &&pattern, getter_type &&getter)
      : KeywordPatternLink(pattern, getter, {}) {}

  KeywordPatternLink(StringType &&pattern, setter_type &&setter)
      : KeywordPatternLink(pattern, {}, setter) {}

  KeywordPatternLink(StringType &&pattern, getter_type &&getter,
                     setter_type &&setter)
      : m_key(pattern), m_setter{setter}, m_getter{getter} {}

  void match(StringType s) {
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

#endif // !__SIMPLEPARSER_H_