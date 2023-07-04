#pragma once

#include <array>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "InputStream.h"

namespace vpktool {

constexpr std::uint32_t VPK_ID = 0x55aa1234;
constexpr std::uint32_t VPK_DIR_INDEX = 0x7fff;

struct VPKEntry {
    /// File name of this entry (e.g. "cable.vmt")
    std::string filename;
    /// CRC32 checksum
    std::uint32_t crc32 = 0;
    /// Length in bytes
    std::uint32_t length = 0;
    /// Offset in the VPK
    std::uint32_t offset = 0;
    /// Which VPK this entry is in
    std::uint16_t archiveIndex = 0;
    /// Preloaded data
    std::vector<std::byte> preloadedData;

private:
    friend class VPK;
    VPKEntry() = default;
};

class VPK {
private:
#pragma pack(push, 1)
    struct Header1 {
        std::uint32_t signature;
        std::uint32_t version;
        std::uint32_t treeSize;
    };

    struct Header2 {
        std::uint32_t fileDataSectionSize;
        std::uint32_t archiveMD5SectionSize;
        std::uint32_t otherMD5SectionSize;
        std::uint32_t signatureSectionSize;
    };

    struct Footer2 {
        std::array<std::byte, 16> treeChecksum{};
        std::array<std::byte, 16> md5EntriesChecksum{};
        std::array<std::byte, 16> wholeFileChecksum{};
        /// If the public key size is VPK_ID it's a CS2 vpk (and probably has no key or signature?)
        bool cs2VPK = false;
        std::vector<std::byte> publicKey{};
        std::vector<std::byte> signature{};
    };

    struct MD5Entry {
        /// The CRC32 checksum of this entry.
        std::uint32_t archiveIndex;
        /// The offset in the package.
        std::uint32_t offset;
        /// The length in bytes.
        std::uint32_t length;
        /// The expected Checksum checksum.
        std::array<std::byte, 16> checksum;
    };
#pragma pack(pop)

public:
    VPK();
    VPK(const VPK& other) = delete;
    VPK& operator=(const VPK& other) = delete;
    VPK(VPK&& other) noexcept = default;
    VPK& operator=(VPK&& other) noexcept = default;

    /// Open a directory VPK file
    [[nodiscard]] static std::optional<VPK> open(const std::string& path);

    /// Open a directory VPK from memory
    /// Note that any content not stored in the directory VPK will fail to load!
    [[nodiscard]] static std::optional<VPK> open(std::byte* buffer, std::uint64_t bufferLen);

    [[nodiscard]] std::optional<VPKEntry> findEntry(const std::string& filename_) const;
    [[nodiscard]] std::optional<VPKEntry> findEntry(const std::string& directory, const std::string& filename_) const;

    [[nodiscard]] std::vector<std::byte> readBinaryEntry(const VPKEntry& entry) const;
    [[nodiscard]] std::string readTextEntry(const VPKEntry& entry) const;

    [[nodiscard]] const std::unordered_map<std::string, std::vector<VPKEntry>>& getEntries() const {
        return this->entries;
    }

    [[nodiscard]] std::uint32_t getHeaderLength() const {
        if (!this->header2.fileDataSectionSize) {
            return sizeof(Header1);
        }
        return sizeof(Header1) + sizeof(Header2);
    }

    [[nodiscard]] std::string_view getPrettyFileName() const {
        // Find the last occurrence of the slash character
        if (std::size_t lastSlashIndex = this->filename.find_last_of('/'); lastSlashIndex != std::string::npos) {
            return {this->filename.data() + lastSlashIndex + 1, this->filename.length() - lastSlashIndex - 1};
        }
        return this->filename; // not much else to do, should never happen
    }

protected:
    VPK(InputStream&& reader_, std::string filename_);

    std::string filename;

    Header1 header1{}; // Present in all VPK versions
    Header2 header2{}; // Present in VPK v2
    Footer2 footer2{}; // Present in VPK v2

    std::unordered_map<std::string, std::vector<VPKEntry>> entries;
    std::vector<MD5Entry> md5Entries;

    InputStream reader;

private:
    [[nodiscard]] static bool open(VPK& vpk);
};

} // namespace vpktool
