#pragma once
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace Valve {
    namespace ValveFileFormat {

        class KeyValue
        {
        public:
            KeyValue(const std::string& key, const std::string& value);
            KeyValue(std::tuple<std::string, std::string>&& data);
            const std::string& Key() const;
            const std::string& Value() const;

        private:
            std::string m_Key;
            std::string m_Value;
        };

        class Node
        {
        public:
            Node(const std::string& name);
            void AddChild(Node&& child);
            void AddProperty(KeyValue&& prop);
            void AddProperty(std::tuple<std::string, std::string>&& data);
            const std::vector<Node>& GetChildren() const;
            //const std::vector<KeyValue>& GetProperties() const;
            const std::unordered_map<std::string, std::string>& GetProperties() const;
            const std::string& Name() const;

        private:
            std::string           m_Name;
            std::vector<Node>     m_cChilds;
            //std::vector<KeyValue> m_cProperties;
            std::unordered_map<std::string, std::string> m_cProperties;
        };

        class Parser
        {
        public:
            enum EFileEncoding
            {
                ENC_UTF8 = 0,
                ENC_UTF16_LE,
                ENC_UTF16_BE,
                ENC_UTF16_CONSUME,
                ENC_MAX
            };

        public:
            Parser() = default;
            std::unique_ptr<Node> Parse(const std::string& path, EFileEncoding enc = ENC_UTF8);

        private:
            std::string GetLine();
            Node ParseNode(const std::string& line);
            bool LoadFile(const std::string& path, EFileEncoding enc = ENC_UTF8);

        private:
            static std::string wstring_to_string(const std::wstring& wstr);
            static std::vector<size_t> GetQuotePositions(const std::string& line);
            static bool IsNode(const std::string& line);
            static bool IsProperty(const std::string& line);
            static std::string ExtractNodeName(const std::string& line);
            static std::tuple<std::string, std::string> ExtractKeyValue(const std::string& line);

        private:
            std::stringstream m_ss;
        };

    } // namespace ValveFileFormat
} // namespace Valve