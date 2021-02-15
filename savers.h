//
// Created by xflajs00 on 04.02.2021.
//

#ifndef VITEJTE_POSTGRESQL_SERVICE__SAVERS_H
#define VITEJTE_POSTGRESQL_SERVICE__SAVERS_H

#include "lib_interface/Communicator.h"
#include <memory>
#include <pqxx/pqxx>
#include <toml++/toml.h>

namespace vitejte {
class VitejteDataSaver {
 public:
  virtual void saveVisit(const Patient &patient) = 0;
  virtual void updateVisit(const Patient &patient) = 0;
  [[nodiscard]] virtual bool isSaved(const Patient &patient) = 0;

  void saveOrUpdate(const Patient &patient);
  virtual ~VitejteDataSaver() = default;
};

class MockSaver : public VitejteDataSaver {
 public:
  MockSaver() = default;
  void saveVisit(const Patient &patient) override;
  void updateVisit(const Patient &patient) override;
  bool isSaved(const Patient &patient) override;

};

struct PostgresConnectionInfo {
  std::string user;
  std::string password;
  std::string host;
  uint16_t port;
  std::string dbName;

  explicit PostgresConnectionInfo(const toml::table &src);

  [[nodiscard]] std::string toString() const;
};

class PostgresSaver : public VitejteDataSaver {
  constexpr static auto INSERT_SQL = R"sql(insert into "Patient" values($1, $2))sql";
  constexpr static auto UPDATE_SQL = R"sql(update "Patient" SET name = $1 where id = $2)sql";
  constexpr static auto SELECT_SQL = R"sql(select id, name from "Patient" where id = $1)sql";

 public:
  explicit PostgresSaver(const PostgresConnectionInfo &info);

  void saveVisit(const vitejte::Patient &patient) override;
  void updateVisit(const vitejte::Patient &patient) override;
  [[nodiscard]] bool isSaved(const Patient &patient) override;

 private:
  pqxx::connection connection;

  pqxx::work makeTransaction();
};

void saveOrUpdate(VitejteDataSaver &saver, const Patient &patient);

enum class SaverType { postgres, mock, none };

std::unique_ptr<VitejteDataSaver> createDataSaver(SaverType type, toml::table &config);

int runSavingTest(vitejte::VitejteDataSaver &saver);
}// namespace vitejte

#endif//VITEJTE_POSTGRESQL_SERVICE__SAVERS_H
