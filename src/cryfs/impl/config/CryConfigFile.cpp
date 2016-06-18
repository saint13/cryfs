#include "CryConfigFile.h"
#include <fstream>
#include <boost/filesystem.hpp>
#include <sstream>
#include <cpp-utils/logging/logging.h>

using boost::optional;
using boost::none;
using std::ifstream;
using std::ofstream;
using std::string;
using std::istringstream;
using std::ostringstream;
using std::stringstream;
using std::istream;
using cpputils::Data;
using cpputils::unique_ref;
using cpputils::make_unique_ref;
using cpputils::SCryptSettings;
using cpputils::either;
namespace bf = boost::filesystem;
using namespace cpputils::logging;

namespace cryfs {

CryConfigFile::~CryConfigFile() {
    //We do not call save() here, because we do not want the config file to be re-encrypted on each filesystem run
}

either<CryConfigFile::LoadError, unique_ref<CryConfigFile>> CryConfigFile::load(const bf::path &path, const string &password) {
    auto encryptedConfigData = Data::LoadFromFile(path);
    if (encryptedConfigData == none) {
        return LoadError::ConfigFileNotFound;
    }
    auto encryptor = CryConfigEncryptorFactory::loadKey(*encryptedConfigData, password);
    if (encryptor == none) {
        return LoadError::DecryptionFailed;
    }
    auto decrypted = (*encryptor)->decrypt(*encryptedConfigData);
    if (decrypted == none) {
        return LoadError::DecryptionFailed;
    }
    CryConfig config = CryConfig::load(decrypted->data);
    if (config.Cipher() != decrypted->cipherName) {
        LOG(ERROR) << "Inner cipher algorithm used to encrypt config file doesn't match config value";
        return LoadError::DecryptionFailed;
    }
    auto configFile = make_unique_ref<CryConfigFile>(CryConfigFile(path, std::move(config), std::move(*encryptor)));
    if (decrypted->wasInDeprecatedConfigFormat) {
        // Migrate it to new format
        configFile->save();
    }
    //TODO For newer compilers, this works without std::move
    return std::move(configFile);
}

unique_ref<CryConfigFile> CryConfigFile::create(const bf::path &path, CryConfig config, const string &password, const SCryptSettings &scryptSettings) {
    if (bf::exists(path)) {
        throw std::runtime_error("Config file exists already.");
    }
    auto result = make_unique_ref<CryConfigFile>(CryConfigFile(path, std::move(config), CryConfigEncryptorFactory::deriveKey(password, scryptSettings)));
    result->save();
    return result;
}

CryConfigFile::CryConfigFile(const bf::path &path, CryConfig config, unique_ref<CryConfigEncryptor> encryptor)
    : _path (path), _config(std::move(config)), _encryptor(std::move(encryptor)) {
}

void CryConfigFile::save() const {
    Data configData = _config.save();
    auto encrypted = _encryptor->encrypt(configData, _config.Cipher());
    encrypted.StoreToFile(_path);
}

CryConfig *CryConfigFile::config() {
    return const_cast<CryConfig*>(const_cast<const CryConfigFile*>(this)->config());
}

const CryConfig *CryConfigFile::config() const {
    return &_config;
}

}