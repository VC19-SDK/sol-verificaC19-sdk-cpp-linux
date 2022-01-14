// Copyright (c) 2021 Solari di Udine S.p.A
// Licensed under the Apache License, Version 2.0

#include <cstring>
#include <sstream>
#include <fstream>
#include <crl-storage/CRLStorageMemory.hpp>

namespace verificaC19Sdk {

CRLStorageMemory::CRLStorageMemory() : m_updatingMode(false) {
}

bool CRLStorageMemory::hasCertificateHash(const std::string& certificateHash) const {
	if (!m_CRL.isMember("list")) {
		return false;
	}
	return m_CRL["list"].isMember(certificateHash);
}

void CRLStorageMemory::beginUpdatingCRL() {
	m_updatingMode = true;
	m_updatingCRL = m_CRL;
}

void CRLStorageMemory::storeChunk(const std::string& chunk) {
	Json::Value jCRL;
	if (m_updatingMode) {
		jCRL = m_updatingCRL;
	} else {
		jCRL = m_CRL;
	}
	Json::CharReaderBuilder builder;
	Json::CharReader* reader = builder.newCharReader();
	Json::Value jChunk;
	std::string errors;
	reader->parse(chunk.c_str(), chunk.c_str() + chunk.length(), &jChunk, &errors);
	delete reader;
	Json::Value jDownloadInfo;
	jDownloadInfo["id"] = jChunk["id"];
	jDownloadInfo["fromVersion"] = jChunk["fromVersion"];
	jDownloadInfo["version"] = jChunk["version"];
	if (jChunk.isMember("chunk")) {
		jDownloadInfo["chunk"] = jChunk["chunk"];
	}
	if (jChunk.isMember("lastChunk")) {
		jDownloadInfo["lastChunk"] = jChunk["lastChunk"];
	}
	jDownloadInfo["creationDate"] = jChunk["creationDate"];
	jDownloadInfo["sizeSingleChunkInByte"] = jChunk["sizeSingleChunkInByte"];
	jDownloadInfo["totalNumberUCVI"] = jChunk["totalNumberUCVI"];
	jCRL["downloadInfo"] = jDownloadInfo;
	if (!jCRL.isMember("list")) {
		jCRL["list"] = Json::Value();
	}
	Json::Value insertions = Json::Value(Json::arrayValue);
	Json::Value deletions = Json::Value(Json::arrayValue);
	if (jChunk.isMember("revokedUcvi")) {
		insertions = jChunk["revokedUcvi"];
	} else {
		insertions = jChunk["delta"]["insertions"];
		deletions = jChunk["delta"]["deletions"];
	}
	if (insertions.size() > 0) {
		for (Json::ArrayIndex idx = 0; idx < insertions.size(); idx++) {
			jCRL["list"][insertions[idx].asString()] = "";
		}
	}
	if (deletions.size() > 0) {
		for (Json::ArrayIndex idx = 0; idx < deletions.size(); idx++) {
			jCRL["list"].removeMember(deletions[idx].asString());
		}
	}
	if (!m_updatingMode) {
		m_CRL = jCRL;
		commitUpdatedCRL();
	} else {
		m_updatingCRL = jCRL;
	}
}

void CRLStorageMemory::clearCRL() {
	if (m_updatingMode) {
		m_updatingCRL.clear();
	} else {
		m_CRL.clear();
		commitUpdatedCRL();
	}
}

void CRLStorageMemory::commitUpdatedCRL() {
	if (m_updatingMode) {
		m_CRL = m_updatingCRL;
		m_updatingCRL.clear();
	}
	m_updatingMode = false;
	m_CRL["_lastUpdate"] = (int)time(NULL);
	m_CRL["version"] = m_CRL["downloadInfo"]["version"];
	m_CRL.removeMember("downloadInfo");
}

void CRLStorageMemory::rollbackUpdatedCRL() {
	if (m_updatingMode) {
		m_updatingCRL.clear();
		m_updatingMode = false;
	} else {
		m_CRL.clear();
		commitUpdatedCRL();
	}
}

std::string CRLStorageMemory::downloadInfo() const {
	if (!m_CRL.isMember("downloadInfo")) {
		return "";
	}
	std::stringstream sDownloadInfo;
	Json::StreamWriterBuilder builder;
	Json::StreamWriter* fastWriter = builder.newStreamWriter();
	fastWriter->write(m_CRL["downloadInfo"], &sDownloadInfo);
	delete fastWriter;
	return sDownloadInfo.str();
}

} // namespace verificaC19Sdk

