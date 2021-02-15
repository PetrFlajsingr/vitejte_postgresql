//
// Created by xflajs00 on 04.02.2021.
//

#include "savers.h"
#include "global_logger.h"
#include "utils.h"

using namespace std::string_literals;

namespace vitejte {

void VitejteDataSaver::saveOrUpdate(const Patient &patient) {
  if (isSaved(patient)) {
    updateVisit(patient);
  } else {
    saveVisit(patient);
  }
}

PostgresConnectionInfo::PostgresConnectionInfo(const toml::table &src)
    : user(getOrThrow<std::string>(src, "user")), password(getOrThrow<std::string>(src, "password")),
      host(getOrThrow<std::string>(src, "host")), port(getOrThrow<uint16_t>(src, "port")),
      dbName(getOrThrow<std::string>(src, "dbName")) {}

std::string PostgresConnectionInfo::toString() const {
  return "postgresql://"s + user + ":" + password + "@" + host + ":" + std::to_string(port) + "/" + dbName;
}

PostgresSaver::PostgresSaver(const PostgresConnectionInfo &info) : connection(info.toString()) {
  logger->log(spdlog::level::info, "Created postgres connection");
  connection.prepare("insert", INSERT_SQL);
  connection.prepare("update", UPDATE_SQL);
  connection.prepare("select", SELECT_SQL);
  logger->log(spdlog::level::trace, "Prepared statements");
}

void PostgresSaver::saveVisit(const Patient &patient) {
  logger->log(spdlog::level::info, "Saving patient visit");
  auto transaction = makeTransaction();
  transaction.exec_prepared("insert", patient.getId(), patient.getName() + " " + patient.getLastName());
  transaction.commit();
  logger->log(spdlog::level::trace, "Transaction comit");
}

void PostgresSaver::updateVisit(const Patient &patient) {
  logger->log(spdlog::level::info, "Updating patient visit");
  auto transaction = makeTransaction();
  transaction.exec_prepared("update", patient.getName() + " " + patient.getLastName(), patient.getId());
  transaction.commit();
  logger->log(spdlog::level::trace, "Transaction comit");
}

bool PostgresSaver::isSaved(const Patient &patient) {
  auto transaction = makeTransaction();
  auto result = transaction.exec_prepared("select", patient.getId());
  return !result.empty();
}

pqxx::work PostgresSaver::makeTransaction() {
  logger->log(spdlog::level::trace, "Creating transaction");
  return pqxx::work{connection};
}

std::unique_ptr<VitejteDataSaver> createDataSaver(SaverType type, toml::table &config) {
  switch (type) {
    case SaverType::postgres: {
      auto postgresConfig = config["postgres"];
      if (!postgresConfig.is_table()) { throw std::runtime_error("postgres config missing"); }

      return std::make_unique<PostgresSaver>(PostgresConnectionInfo(*postgresConfig.as_table()));
    }
    case SaverType::mock: return std::make_unique<MockSaver>();
  }
  throw std::logic_error("Saver type not implemented");
}

int runSavingTest(VitejteDataSaver &saver) {
  logger->log(spdlog::level::debug, "Running test");
  try {
    auto rawPatient = vitejte::RawPatient{};
    rawPatient.id = 1;
    rawPatient.registrationTime = 0;
    rawPatient.clearanceTime = 0;
    rawPatient.titlePre = "";
    rawPatient.titlePost = "";
    rawPatient.name = "test1";
    rawPatient.lastName = "test1";
    rawPatient.rc = "1234567890";
    rawPatient.zp = "111";
    rawPatient.duvod = "test_reason";
    rawPatient.idFronty = 1;
    rawPatient.state = static_cast<int>(vitejte::Patient::State::Added);
    rawPatient.language = "ces";
    rawPatient.cardType = static_cast<int>(vitejte::Patient::CardType::Insurance);
    const auto patient1 = vitejte::Patient{rawPatient};
    saver.saveOrUpdate(patient1);

    rawPatient.id = 24545454;
    rawPatient.registrationTime = 0;
    rawPatient.clearanceTime = 0;
    rawPatient.titlePre = "";
    rawPatient.titlePost = "";
    rawPatient.name = "test2";
    rawPatient.lastName = "test2";
    rawPatient.rc = "1234567890";
    rawPatient.zp = "111";
    rawPatient.duvod = "test_reason";
    rawPatient.idFronty = 1;
    rawPatient.state = static_cast<int>(vitejte::Patient::State::Added);
    rawPatient.language = "ces";
    rawPatient.cardType = static_cast<int>(vitejte::Patient::CardType::Insurance);
    auto patient2 = vitejte::Patient{rawPatient};
    saver.saveOrUpdate(patient2);

    rawPatient.state = static_cast<int>(vitejte::Patient::State::Cleared);
    patient2 = vitejte::Patient{rawPatient};
    saver.saveOrUpdate(patient2);

  } catch (pqxx::sql_error const &e) {
    logger->log(spdlog::level::err, "SQL error: {}, query: {}", e.what(), e.query());
    return 2;
  } catch (std::exception const &e) {
    logger->log(spdlog::level::err, "Error: {}", e.what());
    return 1;
  }
  return 0;
}
void MockSaver::saveVisit(const Patient &patient) { logger->log(spdlog::level::debug, "MockSaver::saveVisit"); }
void MockSaver::updateVisit(const Patient &patient) { logger->log(spdlog::level::debug, "MockSaver::updateVisit"); }
bool MockSaver::isSaved(const Patient &patient) {
  logger->log(spdlog::level::debug, "MockSaver::isSaved");
  return false;
}

}// namespace vitejte