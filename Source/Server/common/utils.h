//add at the end of file
template<class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
inline bool str_to_enum(T& out, const char* in) {
    using S = std::underlying_type_t<T>;
    S temp{};
    if (!str_to_number(temp, in))
        return false;
    out = static_cast<T>(temp);
    return true;
}

template <size_t size>
inline bool str_to_cstring(char(&array)[size], const char* from) {
    auto len = strnlen(from, size - 1) + 1;
    strncpy(array, from, len);
    array[size - 1] = 0;
    return true;
}

inline bool str_to_string(std::string& dest, const char* from) {
    auto len = strnlen(from, 4096);
    dest = std::string{ from, len };
    return true;
}