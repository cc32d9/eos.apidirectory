/*
  Copyright 2019 cc32d9@gmail.com

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include <eosio/eosio.hpp>
#include <eosio/multi_index.hpp>


using namespace eosio;
using std::string;


CONTRACT wipeout : public eosio::contract {
 public:
  wipeout( name self, name code, datastream<const char*> ds ):
    contract(self, code, ds)
      {}

  ACTION wipeaux(uint32_t count)
  {
    require_auth(_self);
    uint32_t orig_count = count;

    {
      apitypes _apitypes(_self, _self.value);
      auto itr = _apitypes.begin();
      while(itr != _apitypes.end() && count > 0) {
        itr = _apitypes.erase(itr);
        --count;
      }
    }
    
    {
      providers _providers(_self, _self.value);
      auto itr = _providers.begin();
      while(itr != _providers.end() && count > 0) {
        itr = _providers.erase(itr);
        --count;
      }
    }

    {
      props _props(_self, _self.value);
      auto itr = _props.begin();
      while(itr != _props.end() && count > 0) {
        itr = _props.erase(itr);
        --count;
      }
    }
    
    check(orig_count > count, "Nothing to wipe");
  }

  
  ACTION wipeentries(uint32_t count)
  {
    require_auth(_self);
    uint32_t orig_count = count;
    
    networks _networks(_self, _self.value);
    auto netitr = _networks.begin();
    
    while(netitr != _networks.end() && count > 0) {
      records _records(_self, netitr->netname.value);
      auto recitr = _records.begin();
      while( recitr != _records.end() && count > 0) {
        recitr = _records.erase(recitr);
        --count;
      }

      if( recitr == _records.end() ) {
        netitr = _networks.erase(netitr);
      }
    }

    check(orig_count > count, "Nothing to wipe");
  }



 private:

  struct [[eosio::table("networks")]] network {
    name         netname;
    checksum256  chainid;
    string       description;
    string       url;
    auto primary_key()const { return netname.value; }
  };

  typedef eosio::multi_index<name("networks"), network> networks;


  struct [[eosio::table("apitypes")]] apitype {
    name         type;
    string       description;
    string       url;
    auto primary_key()const { return type.value; }
  };

  typedef eosio::multi_index<name("apitypes"), apitype> apitypes;


  struct [[eosio::table("providers")]] provider {
    name         provider;
    bool         approved;
    string       url;
    string       email;
    string       im;
    auto primary_key()const { return provider.value; }
  };

  typedef eosio::multi_index<name("providers"), provider> providers;


  struct [[eosio::table("auditors")]] auditor {
    name         auditor;
    string       contact_name;
    string       pgp_key_fingerprint;
    string       pgp_key_server;
    string       email;
    string       im;
    auto primary_key()const { return auditor.value; }
  };

  typedef eosio::multi_index<name("auditors"), auditor> auditors;


  inline static uint128_t recidx(name type, name provider) {
    return (((uint128_t)type.value)<<64) | provider.value;
  }

  // network name is the scope
  struct [[eosio::table("records")]] record {
    uint64_t        id;
    name            type;
    name            provider;
    name            srvname;
    string          url;
    string          continent;
    string          country;
    uint32_t        flags;
    uint32_t        revision;
    time_point_sec  updated_on;
    name            audited_by;
    time_point_sec  audited_on;
    string          audit_ipfs_file;
    auto primary_key()const { return id; }
    uint64_t get_type() const { return type.value; }
    uint128_t get_recidx() const { return recidx(type, provider); }
  };

  typedef eosio::multi_index<name("records"), record,
                             indexed_by<name("type"), const_mem_fun<record, uint64_t, &record::get_type>>,
                             indexed_by<name("recidx"), const_mem_fun<record, uint128_t, &record::get_recidx>>
                             > records;


  struct [[eosio::table("props")]] prop {
    name         property;
    uint64_t     val_uint;
    string       val_str;
    name         val_name;
    auto primary_key()const { return property.value; }
  };

  typedef eosio::multi_index<name("props"), prop> props;



};



