#include "pch.h"

#include "sdk/KeyValues/ValveKeyValues.h"

#include <fstream>
#include <codecvt>

Valve::ValveFileFormat::KeyValue::KeyValue(const std::string& key, const std::string& value) : m_Key(key), m_Value(value)
{
}

Valve::ValveFileFormat::KeyValue::KeyValue(std::tuple<std::string, std::string>&& data)
    : m_Key(std::get<0>(data)), m_Value(std::get<1>(data))
{
}

const std::string& Valve::ValveFileFormat::KeyValue::Key() const
{
    return m_Key;
}

const std::string& Valve::ValveFileFormat::KeyValue::Value() const
{
    return m_Value;
}

Valve::ValveFileFormat::Node::Node(const std::string& name) : m_Name(name)
{
}

void Valve::ValveFileFormat::Node::AddChild(Node&& child)
{
    m_cChilds.push_back(std::move(child));
}

void Valve::ValveFileFormat::Node::AddProperty(KeyValue&& prop)
{
    m_cProperties[prop.Key()] = prop.Value();
}

void Valve::ValveFileFormat::Node::AddProperty(std::tuple<std::string, std::string>&& data)
{
    m_cProperties[std::get<0>(data)] = std::get<1>(data);
}

const std::vector<Valve::ValveFileFormat::Node>& Valve::ValveFileFormat::Node::GetChildren() const
{
    return m_cChilds;
}

const std::unordered_map<std::string, std::string>& Valve::ValveFileFormat::Node::GetProperties() const
{
    return m_cProperties;
}

const std::string& Valve::ValveFileFormat::Node::Name() const
{
    return m_Name;
}

std::unique_ptr<Valve::ValveFileFormat::Node> Valve::ValveFileFormat::Parser::Parse(const std::string& path, EFileEncoding enc)
{
    if (!LoadFile(path, enc))
        return nullptr;

    auto node = ParseNode(GetLine());
    if (node.GetChildren().size() == 0 && node.GetProperties().size() == 0)
        return nullptr;

    m_ss.clear();
    return std::make_unique<Node>(std::move(node));
}

std::string Valve::ValveFileFormat::Parser::GetLine()
{
    if (!m_ss.good())
        return "";

    std::string line;
    getline(m_ss, line);
    if (line.length() && line.back() == '\n')
        line.pop_back();
    return line;
}

std::string Valve::ValveFileFormat::Parser::wstring_to_string(const std::wstring& wstr)
{
    // old implementation
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);

    //int count = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
    //std::string str(count, 0);
    //WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    //return str;
}

std::vector<size_t> Valve::ValveFileFormat::Parser::GetQuotePositions(const std::string& line)
{
    std::vector<size_t> cPositions;
    auto           bEscape = false;
    for (size_t i = 0; i < line.size(); ++i) {
        auto c = line[i];
        if (c == '\\') {
            bEscape = bEscape ? false : true;
            continue;
        }
        if (c == '"' && !bEscape)
            cPositions.push_back(i);
        bEscape = false;
    }
    return cPositions;
}

bool Valve::ValveFileFormat::Parser::IsNode(const std::string& line)
{
    return GetQuotePositions(line).size() == 2;
}

bool Valve::ValveFileFormat::Parser::IsProperty(const std::string& line)
{
    return GetQuotePositions(line).size() == 4;
}

std::string Valve::ValveFileFormat::Parser::ExtractNodeName(const std::string& line)
{
    auto cPos = GetQuotePositions(line);
    if (cPos.size() != 2)
        return line;
    return line.substr(cPos[0] + 1, cPos[1] - cPos[0] - 1);
}

std::tuple<std::string, std::string> Valve::ValveFileFormat::Parser::ExtractKeyValue(const std::string& line)
{
    auto cPos = GetQuotePositions(line);
    if (cPos.size() != 4)
        return std::make_tuple("Error", line);
    auto key = line.substr(cPos[0] + 1, cPos[1] - cPos[0] - 1);
    auto val = line.substr(cPos[2] + 1, cPos[3] - cPos[2] - 1);
    return make_tuple(key, val);
}

Valve::ValveFileFormat::Node Valve::ValveFileFormat::Parser::ParseNode(const std::string& line)
{
    if (!m_ss.good())
        return { "Error" };

    Node node(ExtractNodeName(line));
    GetLine();
    while (m_ss.good()) {
        auto strLine = GetLine();
        if (IsNode(strLine))
            node.AddChild(ParseNode(strLine));
        else if (IsProperty(strLine))
            node.AddProperty(ExtractKeyValue(strLine));
        else if (strLine.find('}') != std::string::npos)
            return node;
    }

    return node;
}

bool Valve::ValveFileFormat::Parser::LoadFile(const std::string& path, EFileEncoding enc)
{
    using BufIt = std::istreambuf_iterator<char>;
    std::ifstream FileStream(path);
    if (!FileStream || !FileStream.good()) {
        //tfm::printf(XorStr("[!] Could not open file '%s'"), path);
        return false;
    }

    // TODO: reenable if we ever need to open files with different encoding
    /*switch (enc) {
    case ENC_UTF8:
        FileStream.imbue(std::locale(FileStream.getloc(), new std::codecvt_utf8_utf16<wchar_t>));
        break;
    case ENC_UTF16_LE:
        FileStream.imbue(
            std::locale(FileStream.getloc(), new std::codecvt_utf16<wchar_t, 0x10FFFF, std::little_endian>));
        break;
    case ENC_UTF16_BE:
        FileStream.imbue(std::locale(FileStream.getloc(), new std::codecvt_utf16<wchar_t, 0x10FFFF>));
        break;
    default:
    case ENC_UTF16_CONSUME:
        FileStream.imbue(
            std::locale(FileStream.getloc(), new std::codecvt_utf16<wchar_t, 0x10FFFF, std::consume_header>));
        break;
    }*/

    std::string temp(BufIt(FileStream.rdbuf()), BufIt());
    // read data into wstring
    /*std::wstring fileContents;
    FileStream.seekg(0, std::ios::end);
    fileContents.resize(FileStream.tellg());
    FileStream.seekg(0, std::ios::beg);
    FileStream.read(fileContents.data(), fileContents.size());*/

    m_ss << temp;//wstring_to_string(fileContents);
    return true;
}