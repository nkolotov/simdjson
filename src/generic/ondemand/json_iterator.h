namespace {
namespace SIMDJSON_IMPLEMENTATION {
namespace ondemand {

/**
 * Iterates through JSON, with structure-sensitive algorithms.
 * 
 * @private This is not intended for external use.
 */
class json_iterator : public token_iterator {
public:
  simdjson_really_inline json_iterator() noexcept;
  simdjson_really_inline json_iterator(json_iterator &&other) noexcept;
  simdjson_really_inline json_iterator &operator=(json_iterator &&other) noexcept;
  simdjson_really_inline json_iterator(const json_iterator &other) noexcept = delete;
  simdjson_really_inline json_iterator &operator=(const json_iterator &other) noexcept = delete;
  simdjson_really_inline ~json_iterator() noexcept;

  /**
   * Check for an opening { and start an object iteration.
   *
   * @returns Whether the object had any fields (returns false for empty).
   * @error INCORRECT_TYPE if there is no opening {
   */
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<bool> start_object() noexcept;

  /**
   * Start an object iteration after the user has already checked and moved past the {.
   *
   * Does not move the iterator.
   * 
   * @returns Whether the object had any fields (returns false for empty).
   */
  SIMDJSON_WARN_UNUSED simdjson_really_inline bool started_object() noexcept;

  /**
   * Moves to the next field in an object.
   * 
   * Looks for , and }. If } is found, the object is finished and the iterator advances past it.
   * Otherwise, it advances to the next value.
   * 
   * @return whether there is another field in the object.
   * @error TAPE_ERROR If there is a comma missing between fields.
   */
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<bool> has_next_field() noexcept;

  /**
   * Get the current field's key.
   */
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<raw_json_string> field_key() noexcept;

  /**
   * Pass the : in the field and move to its value.
   */
  SIMDJSON_WARN_UNUSED simdjson_really_inline error_code field_value() noexcept;

  /**
   * Find the next field with the given key.
   *
   * Assumes you have called next_field() or otherwise matched the previous value.
   * 
   * Key is *raw JSON,* meaning it will be matched against the verbatim JSON without attempting to
   * unescape it. This works well for typical ASCII and UTF-8 keys (almost all of them), but may
   * fail to match some keys with escapes (\u, \n, etc.).
   */
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<bool> find_field_raw(const char *key) noexcept;

  /**
   * Check for an opening [ and start an array iteration.
   *
   * @returns Whether the array had any elements (returns false for empty).
   * @error INCORRECT_TYPE If there is no [.
   */
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<bool> start_array() noexcept;

  /**
   * Start an array iteration after the user has already checked and moved past the [.
   *
   * Does not move the iterator.
   *
   * @returns Whether the array had any elements (returns false for empty).
   */
  SIMDJSON_WARN_UNUSED simdjson_really_inline bool started_array() noexcept;

  /**
   * Moves to the next element in an array.
   * 
   * Looks for , and ]. If ] is found, the array is finished and the iterator advances past it.
   * Otherwise, it advances to the next value.
   * 
   * @return Whether there is another element in the array.
   * @error TAPE_ERROR If there is a comma missing between elements.
   */
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<bool> has_next_element() noexcept;

  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<raw_json_string> get_raw_json_string() noexcept;
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<uint64_t> get_uint64() noexcept;
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<int64_t> get_int64() noexcept;
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<double> get_double() noexcept;
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<bool> get_bool() noexcept;
  simdjson_really_inline bool is_null() noexcept;

  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<uint64_t> get_root_uint64() noexcept;
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<int64_t> get_root_int64() noexcept;
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<double> get_root_double() noexcept;
  SIMDJSON_WARN_UNUSED simdjson_really_inline simdjson_result<bool> get_root_bool() noexcept;
  simdjson_really_inline bool root_is_null() noexcept;

  /**
   * Skips a JSON value, whether it is a scalar, array or object.
   */
  simdjson_really_inline void skip() noexcept;

  /**
   * Skips to the end of a JSON object or array.
   * 
   * @return true if this was the end of an array, false if it was the end of an object.
   */
  simdjson_really_inline bool skip_container() noexcept;

protected:
  simdjson_really_inline json_iterator(ondemand::parser *parser) noexcept;
  template<int N>
  SIMDJSON_WARN_UNUSED simdjson_really_inline bool advance_to_buffer(uint8_t (&buf)[N]) noexcept;
  ondemand::parser *parser{};

  friend class document;
  friend class object;
  friend class array;
  friend class value;
  friend simdjson_really_inline void logger::log_line(const json_iterator &iter, const char *title_prefix, const char *title, std::string_view detail, int delta, int depth_delta) noexcept;
};

/**
 * Represents temporary ownership of a JSON iterator, which can be handed off to a child or released
 * to a parent.
 */
class json_iterator_lease : public json_iterator {
public:
  simdjson_really_inline json_iterator_lease() noexcept;
  simdjson_really_inline json_iterator_lease(json_iterator &previous_iter) noexcept;
  simdjson_really_inline json_iterator_lease(json_iterator_lease &&previous_iter) noexcept;
  simdjson_really_inline json_iterator_lease &operator=(json_iterator_lease &&previous_iter) noexcept;
  json_iterator_lease(const json_iterator_lease &) = delete;
  json_iterator_lease &operator=(const json_iterator_lease &) = delete;

  simdjson_really_inline ~json_iterator_lease() noexcept;

  simdjson_really_inline json_iterator &release() noexcept;

protected:
  json_iterator *previous_iter{};
};

} // namespace ondemand
} // namespace SIMDJSON_IMPLEMENTATION
} // namespace {