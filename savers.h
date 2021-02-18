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
  constexpr static auto INSERT_SQL = R"sql(insert into vitej(id, id_in_device, reg_time, checkin_time, degree_before,
degree_after, first_name, last_name, ident_number, insu_code, reason_visit, id_queue, state, card_type)
values (default, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13))sql";
  constexpr static auto UPDATE_SQL = R"sql(update vitej SET checkin_time = $1, state = $2, first_name = $3,
last_name = $4, ident_number = $5, insu_code = $6, reason_visit = $7  where reg_time > $8 and id_in_device = $9)sql";
  constexpr static auto SELECT_SQL = R"sql(select * from vitej where reg_time > $1 and id_in_device = $2)sql";

 public:
  explicit PostgresSaver(const PostgresConnectionInfo &info);

  void saveVisit(const vitejte::Patient &patient) override;
  void updateVisit(const vitejte::Patient &patient) override;
  [[nodiscard]] bool isSaved(const Patient &patient) override;

 private:
  pqxx::connection connection;
  pqxx::work makeTransaction();

  std::string lastMidnightAsStr();
};

void saveOrUpdate(VitejteDataSaver &saver, const Patient &patient);

enum class SaverType { postgres, mock, none };

std::unique_ptr<VitejteDataSaver> createDataSaver(SaverType type, toml::table &config);

int runSavingTest(vitejte::VitejteDataSaver &saver);
}// namespace vitejte

#endif//VITEJTE_POSTGRESQL_SERVICE__SAVERS_H
