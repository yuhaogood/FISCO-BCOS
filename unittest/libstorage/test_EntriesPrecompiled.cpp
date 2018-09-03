#include <libdevcore/easylog.h>
#include <libethcore/ABI.h>
#include <libprecompiled/PrecompiledContext.h>
#include <libprecompiled/StringFactoryPrecompiled.h>
#include <libstorage/EntriesPrecompiled.h>
#include <libstorage/EntryPrecompiled.h>
#include <libstorage/StateDB.h>
#include <unittest/Common.h>
#include <boost/test/unit_test.hpp>

using namespace dev;
using namespace dev::storage;
using namespace dev::precompiled;
using namespace dev::eth;

namespace test_EntriesPrecompiled {

struct EntriesPrecompiledFixture {
  EntriesPrecompiledFixture() {
    entry = std::make_shared<Entry>();
    entries = std::make_shared<Entries>();
    stringFactoryPrecompiled = std::make_shared<StringFactoryPrecompiled>();
    precompiledContext =
        std::make_shared<dev::precompiled::PrecompiledContext>();
    entriesPrecompiled =
        std::make_shared<dev::precompiled::EntriesPrecompiled>();

    entriesPrecompiled->setEntries(entries);
    entriesPrecompiled->setStringFactoryPrecompiled(stringFactoryPrecompiled);
  }
  ~EntriesPrecompiledFixture() {}

  dev::storage::Entry::Ptr entry;
  dev::storage::Entries::Ptr entries;
  StringFactoryPrecompiled::Ptr stringFactoryPrecompiled;
  dev::precompiled::PrecompiledContext::Ptr precompiledContext;
  dev::precompiled::EntriesPrecompiled::Ptr entriesPrecompiled;
};

BOOST_FIXTURE_TEST_SUITE(EntriesPrecompiled, EntriesPrecompiledFixture)

BOOST_AUTO_TEST_CASE(testBeforeAndAfterBlock) {
  entriesPrecompiled->beforeBlock(precompiledContext);
  entriesPrecompiled->afterBlock(precompiledContext, true);
  BOOST_TEST(entriesPrecompiled->toString(precompiledContext) == "Entries");
}

BOOST_AUTO_TEST_CASE(testEntries) {
  entry->setField("key", "value");
  entries->addEntry(entry);
  entriesPrecompiled->setEntries(entries);
  BOOST_TEST(entriesPrecompiled->getEntries() == entries);
}

BOOST_AUTO_TEST_CASE(testGet) {
  entry->setField("key", "hello");
  entries->addEntry(entry);
  u256 num = u256(0);
  ContractABI abi;
  bytes bint = abi.abiIn("get(int256)", num);
  bytes out =
      entriesPrecompiled->call(precompiledContext, bytesConstRef(&bint));
  Address address;
  abi.abiOut(bytesConstRef(&out), address);
  auto entryPrecompiled = precompiledContext->getPrecompiled(address);

  bytes bstr = abi.abiIn("getString(string)", "key");
  bytes out2 = entryPrecompiled->call(precompiledContext, bytesConstRef(&bstr));
  Address address2;
  abi.abiOut(bytesConstRef(&out2), address2);
  auto stringPrecompiled = precompiledContext->getPrecompiled(address2);
  std::string outstr = stringPrecompiled->toString(precompiledContext);
  BOOST_TEST(entry->getField("key") == outstr);
}

BOOST_AUTO_TEST_CASE(testSize){
  entry->setField("key", "hello");
  entries->addEntry(entry);
  ContractABI abi;
  bytes bint = abi.abiIn("size()");
  bytes out =
      entriesPrecompiled->call(precompiledContext, bytesConstRef(&bint));
  u256 num;
  abi.abiOut(bytesConstRef(&out), num);
  BOOST_TEST(num == u256(1));
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace test_EntriesPrecompiled