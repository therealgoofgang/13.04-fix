#pragma once
#include <string>
#include <map>
#include <vector>
#include <cstdint>

namespace nlohmann {

class json {
private:
    enum class value_t {
        null,
        object,
        array,
        string,
        boolean,
        number_integer,
        number_unsigned,
        number_float,
        discarded
    };
    
    value_t m_type = value_t::null;
    union {
        std::string* m_value_string;
        bool m_value_boolean;
        int64_t m_value_integer;
        uint64_t m_value_unsigned;
        double m_value_float;
        std::map<std::string, json>* m_value_object;
        std::vector<json>* m_value_array;
    };
    
    void cleanup() {
        switch (m_type) {
            case value_t::string:
                delete m_value_string;
                break;
            case value_t::object:
                delete m_value_object;
                break;
            case value_t::array:
                delete m_value_array;
                break;
            default:
                break;
        }
        m_type = value_t::null;
    }
    
public:
    json() : m_type(value_t::null) {}
    
    json(const json& other) : m_type(value_t::null) {
        *this = other;
    }
    
    json(json&& other) noexcept : m_type(value_t::null) {
        *this = std::move(other);
    }
    
    ~json() {
        cleanup();
    }
    
    json& operator=(const json& other) {
        if (this == &other) return *this;
        
        cleanup();
        
        m_type = other.m_type;
        switch (m_type) {
            case value_t::string:
                m_value_string = new std::string(*other.m_value_string);
                break;
            case value_t::boolean:
                m_value_boolean = other.m_value_boolean;
                break;
            case value_t::number_integer:
                m_value_integer = other.m_value_integer;
                break;
            case value_t::number_unsigned:
                m_value_unsigned = other.m_value_unsigned;
                break;
            case value_t::number_float:
                m_value_float = other.m_value_float;
                break;
            case value_t::object:
                m_value_object = new std::map<std::string, json>(*other.m_value_object);
                break;
            case value_t::array:
                m_value_array = new std::vector<json>(*other.m_value_array);
                break;
            default:
                break;
        }
        
        return *this;
    }
    
    json& operator=(json&& other) noexcept {
        if (this == &other) return *this;
        
        cleanup();
        
        m_type = other.m_type;
        switch (m_type) {
            case value_t::string:
                m_value_string = other.m_value_string;
                other.m_value_string = nullptr;
                break;
            case value_t::boolean:
                m_value_boolean = other.m_value_boolean;
                break;
            case value_t::number_integer:
                m_value_integer = other.m_value_integer;
                break;
            case value_t::number_unsigned:
                m_value_unsigned = other.m_value_unsigned;
                break;
            case value_t::number_float:
                m_value_float = other.m_value_float;
                break;
            case value_t::object:
                m_value_object = other.m_value_object;
                other.m_value_object = nullptr;
                break;
            case value_t::array:
                m_value_array = other.m_value_array;
                other.m_value_array = nullptr;
                break;
            default:
                break;
        }
        
        other.m_type = value_t::null;
        return *this;
    }
    
    // Parse from string
    static json parse(const std::string& s) {
        // Simple JSON parser for basic objects
        // This is a minimal implementation that handles the specific JSON used in MFA bypass
        json result;
        result.m_type = value_t::object;
        result.m_value_object = new std::map<std::string, json>();
        
        // Very basic parser that extracts "accessToken", "token", "entitlements_token", "subject"
        size_t pos = s.find("\"accessToken\"");
        if (pos != std::string::npos) {
            pos = s.find(':', pos);
            size_t start = s.find('"', pos + 1);
            size_t end = s.find('"', start + 1);
            if (start != std::string::npos && end != std::string::npos) {
                json token;
                token.m_type = value_t::string;
                token.m_value_string = new std::string(s.substr(start + 1, end - start - 1));
                (*result.m_value_object)["accessToken"] = token;
            }
        }
        
        pos = s.find("\"token\"");
        if (pos != std::string::npos) {
            pos = s.find(':', pos);
            size_t start = s.find('"', pos + 1);
            size_t end = s.find('"', start + 1);
            if (start != std::string::npos && end != std::string::npos) {
                json token;
                token.m_type = value_t::string;
                token.m_value_string = new std::string(s.substr(start + 1, end - start - 1));
                (*result.m_value_object)["token"] = token;
            }
        }
        
        pos = s.find("\"entitlements_token\"");
        if (pos != std::string::npos) {
            pos = s.find(':', pos);
            size_t start = s.find('"', pos + 1);
            size_t end = s.find('"', start + 1);
            if (start != std::string::npos && end != std::string::npos) {
                json token;
                token.m_type = value_t::string;
                token.m_value_string = new std::string(s.substr(start + 1, end - start - 1));
                (*result.m_value_object)["entitlements_token"] = token;
            }
        }
        
        pos = s.find("\"subject\"");
        if (pos != std::string::npos) {
            pos = s.find(':', pos);
            size_t start = s.find('"', pos + 1);
            size_t end = s.find('"', start + 1);
            if (start != std::string::npos && end != std::string::npos) {
                json subject;
                subject.m_type = value_t::string;
                subject.m_value_string = new std::string(s.substr(start + 1, end - start - 1));
                (*result.m_value_object)["subject"] = subject;
            }
        }
        
        pos = s.find("\"puuid\"");
        if (pos != std::string::npos) {
            pos = s.find(':', pos);
            size_t start = s.find('"', pos + 1);
            size_t end = s.find('"', start + 1);
            if (start != std::string::npos && end != std::string::npos) {
                json puuid;
                puuid.m_type = value_t::string;
                puuid.m_value_string = new std::string(s.substr(start + 1, end - start - 1));
                (*result.m_value_object)["puuid"] = puuid;
            }
        }
        
        pos = s.find("\"region\"");
        if (pos != std::string::npos) {
            pos = s.find(':', pos);
            size_t start = s.find('"', pos + 1);
            size_t end = s.find('"', start + 1);
            if (start != std::string::npos && end != std::string::npos) {
                json region;
                region.m_type = value_t::string;
                region.m_value_string = new std::string(s.substr(start + 1, end - start - 1));
                (*result.m_value_object)["region"] = region;
            }
        }
        
        pos = s.find("\"affinities\"");
        if (pos != std::string::npos) {
            pos = s.find("\"live\"", pos);
            if (pos != std::string::npos) {
                pos = s.find(':', pos);
                size_t start = s.find('"', pos + 1);
                size_t end = s.find('"', start + 1);
                if (start != std::string::npos && end != std::string::npos) {
                    json live;
                    live.m_type = value_t::string;
                    live.m_value_string = new std::string(s.substr(start + 1, end - start - 1));
                    (*result.m_value_object)["affinities"]["live"] = live;
                }
            }
        }
        
        pos = s.find("\"sub\"");
        if (pos != std::string::npos) {
            pos = s.find(':', pos);
            size_t start = s.find('"', pos + 1);
            size_t end = s.find('"', start + 1);
            if (start != std::string::npos && end != std::string::npos) {
                json sub;
                sub.m_type = value_t::string;
                sub.m_value_string = new std::string(s.substr(start + 1, end - start - 1));
                (*result.m_value_object)["sub"] = sub;
            }
        }
        
        // Parse riotClientVersion from valorant-api.com response
        pos = s.find("\"riotClientVersion\"");
        if (pos != std::string::npos) {
            pos = s.find(':', pos);
            size_t start = s.find('"', pos + 1);
            size_t end = s.find('"', start + 1);
            if (start != std::string::npos && end != std::string::npos) {
                json version;
                version.m_type = value_t::string;
                version.m_value_string = new std::string(s.substr(start + 1, end - start - 1));
                (*result.m_value_object)["data"]["riotClientVersion"] = version;
            }
        }
        
        return result;
    }
    
    // Check if contains key
    bool contains(const std::string& key) const {
        if (m_type != value_t::object || !m_value_object) return false;
        return m_value_object->find(key) != m_value_object->end();
    }
    
    // Access operators
    json& operator[](const std::string& key) {
        if (m_type != value_t::object) {
            cleanup();
            m_type = value_t::object;
            m_value_object = new std::map<std::string, json>();
        }
        return (*m_value_object)[key];
    }
    
    const json& operator[](const std::string& key) const {
        static json null_json;
        if (m_type != value_t::object || !m_value_object) return null_json;
        auto it = m_value_object->find(key);
        if (it == m_value_object->end()) return null_json;
        return it->second;
    }
    
    // Type checking
    bool is_string() const { return m_type == value_t::string; }
    bool is_boolean() const { return m_type == value_t::boolean; }
    bool is_number() const { return m_type == value_t::number_integer || m_type == value_t::number_unsigned || m_type == value_t::number_float; }
    bool is_object() const { return m_type == value_t::object; }
    bool is_array() const { return m_type == value_t::array; }
    bool is_null() const { return m_type == value_t::null; }
    
    // Get values
    std::string get_string() const {
        if (m_type != value_t::string || !m_value_string) return "";
        return *m_value_string;
    }
    
    bool get_boolean() const {
        if (m_type != value_t::boolean) return false;
        return m_value_boolean;
    }
    
    int64_t get_integer() const {
        if (m_type == value_t::number_integer) return m_value_integer;
        if (m_type == value_t::number_unsigned) return static_cast<int64_t>(m_value_unsigned);
        if (m_type == value_t::number_float) return static_cast<int64_t>(m_value_float);
        return 0;
    }
    
    uint64_t get_unsigned() const {
        if (m_type == value_t::number_unsigned) return m_value_unsigned;
        if (m_type == value_t::number_integer) return static_cast<uint64_t>(m_value_integer);
        if (m_type == value_t::number_float) return static_cast<uint64_t>(m_value_float);
        return 0;
    }
    
    double get_float() const {
        if (m_type == value_t::number_float) return m_value_float;
        if (m_type == value_t::number_integer) return static_cast<double>(m_value_integer);
        if (m_type == value_t::number_unsigned) return static_cast<double>(m_value_unsigned);
        return 0.0;
    }
    
    // String conversion
    std::string dump() const {
        // Simple serialization
        switch (m_type) {
            case value_t::string:
                return "\"" + *m_value_string + "\"";
            case value_t::boolean:
                return m_value_boolean ? "true" : "false";
            case value_t::number_integer:
                return std::to_string(m_value_integer);
            case value_t::number_unsigned:
                return std::to_string(m_value_unsigned);
            case value_t::number_float:
                return std::to_string(m_value_float);
            case value_t::object:
                if (!m_value_object || m_value_object->empty()) return "{}";
                {
                    std::string result = "{";
                    bool first = true;
                    for (const auto& [key, value] : *m_value_object) {
                        if (!first) result += ",";
                        first = false;
                        result += "\"" + key + "\":" + value.dump();
                    }
                    result += "}";
                    return result;
                }
            case value_t::array:
                if (!m_value_array || m_value_array->empty()) return "[]";
                {
                    std::string result = "[";
                    bool first = true;
                    for (const auto& value : *m_value_array) {
                        if (!first) result += ",";
                        first = false;
                        result += value.dump();
                    }
                    result += "]";
                    return result;
                }
            default:
                return "null";
        }
    }
};

} // namespace nlohmann