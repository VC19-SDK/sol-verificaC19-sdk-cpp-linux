// Copyright (c) 2021 Solari di Udine S.p.A
// Licensed under the Apache License, Version 2.0

#ifndef DGC_VERIFIER_HPP
#define DGC_VERIFIER_HPP

#include <verificaC19-sdk/interfaces/IKeysStorage.hpp>
#include <verificaC19-sdk/interfaces/IRulesStorage.hpp>
#include <verificaC19-sdk/interfaces/ICRLStorage.hpp>
#include <verificaC19-sdk/interfaces/ILogger.hpp>
#include <verificaC19-sdk/models/CertificateSimple.hpp>

#define SCAN_MODE_2G   "2G"
#define SCAN_MODE_3G   "3G"
#define SCAN_MODE_STANDARD      SCAN_MODE_3G
#define SCAN_MODE_STRENGTHENED  SCAN_MODE_2G
#define SCAN_MODE_BOOSTER       "BOOSTED"
#define SCAN_MODE_WORK          "WORK"
#define SCAN_MODE_SCHOOL        "SCHOOL"

namespace verificaC19Sdk {

#define DGCVerifier_SDK_Version "1.1.2"

/**
 * Implements Digital Certificate verification
 */
class DGCVerifier {

public:

	/**
	 * @param keysStorage Implementation of Keys Storage service, used to retrieve
	 *                    key needed to verify Digital Certificate validity
	 *
	 * @param rulesStorage Implementation of Rules Storage service, used to retrieve
	 *                     rules needed to verify Digital Certificate validity
	 *
	 * @param crlStorage Implementation of CRL Storage service, used to retrieve
	 *                   crl needed to verify Digital Certificate validity
	 *
	 * @param logger Optional implementation of Logger, if NULL no log will be done
	 */
	DGCVerifier(IKeysStorage* keysStorage, IRulesStorage* rulesStorage,
				ICRLStorage* crlStorage, ILogger* logger);

	/**
	 * Check if minimum SDK version rule is respected
	 *
	 * @return true if minimum SDK version rule is respected, false if needs to update SDK.
	 */
	bool verifyMinSdkVersion() const;

	/**
	 * Verify Digital Certificate
	 *
	 * @param dgcQr Digital Certificate to verify (raw qr code data starting with HC1:)
	 *
	 * @param scanMode Scan mode, SCAN_MODE_STRENGTHENED accept only vaccination and recovery
	 *                 certificates, SCAN_MODE_STANDARD accept also test certificates,
	 *                 SCAN_MODE_BOOSTER signals test needed for complete vaccination (not booster)
	 *                 or for recovery certificate, SCAN_MODE_WORK does not accept test for
	 *                 people aged 50 and over, but only starting from 15 february 2022,
	 *                 SCAN_MODE_SCHOOL
	 *
	 * @return CertificateSimple Result of Digital Certification verify
	 */
	CertificateSimple verify(const std::string& dgcQr, const std::string& scanMode) const;

private:

	IKeysStorage* m_keysStorage;
	IRulesStorage* m_rulesStorage;
	ICRLStorage* m_crlStorage;
	ILogger* m_logger;

}; // class DGCVerifier

extern "C" {

/**
 * DGCVerifier constructor interface for dynamic loading
 */
DGCVerifier* DGCVerifier_create(IKeysStorage* keysStorage, IRulesStorage* rulesStorage, ICRLStorage* crlStorage, ILogger* logger);
typedef DGCVerifier* (*pfDGCVerifier_create)(IKeysStorage*, IRulesStorage*, ICRLStorage*, ILogger*);

/**
 * DGCVerifier destructor interface for dynamic loading
 */
void DGCVerifier_release(DGCVerifier* dgcVerifier);
typedef void (*pfDGCVerifier_release)(DGCVerifier*);

/**
 * DGCVerifier verifyMinSdkVersion interface for dynamic loading
 */
bool DGCVerifier_verifyMinSdkVersion(DGCVerifier* dgcVerifier);
typedef bool (*pfDGCVerifier_verifyMinSdkVersion)(DGCVerifier*);

/**
 * DGCVerifier verify interface for dynamic loading
 */
CertificateSimple DGCVerifier_verify(DGCVerifier* dgcVerifier, const std::string& dgcQr, const std::string& scanMode);
typedef CertificateSimple (*pfDGCVerifier_verify)(DGCVerifier*, const std::string, const std::string);

} // extern "C"

} // namespace verificaC19Sdk

#endif // #ifndef DGC_VERIFIER_HPP
