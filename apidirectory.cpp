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

#include <eosiolib/eosio.hpp>
#include <eosiolib/action.hpp>
#include <eosiolib/multi_index.hpp>


using namespace eosio;
using std::string;

const string iso_continents[] = {"ANY", "AF", "AN", "AS", "EU", "NA", "OC", "SA"};

CONTRACT apidirectory : public eosio::contract {
 public:
  apidirectory( name self, name code, datastream<const char*> ds ):
    contract(self, code, ds),
    _networks(self, self.value),
    _apitypes(self, self.value),
    _providers(self, self.value)
      {}

  
  ACTION setnetwork(name netname, checksum256  chainid, string description, string url)
  {
    require_auth(permission_level(_self, name("admin")));

    auto setter = [&]( auto& item ) {
      item.netname = netname;
      item.chainid = chainid;
      item.description = description;
      item.url = url;
    };
    
    auto netitr = _networks.find(netname.value);
    if( netitr == _networks.end() ) {
      _networks.emplace(_self, setter);
    }
    else {
      _networks.modify(*netitr, _self, setter);
    }
  }


  ACTION setapitype(name type, string description, string url)
  {
    require_auth(permission_level(_self, name("admin")));

    auto setter = [&]( auto& item ) {
      item.type = type;
      item.description = description;
      item.url = url;
    };
    
    auto typitr = _apitypes.find(type.value);
    if( typitr == _apitypes.end() ) {
      _apitypes.emplace(_self, setter);
    }
    else {
      _apitypes.modify(*typitr, _self, setter);
    }
  }
  

  ACTION setprovider(name provider, string url, string email, string im)
  {
    require_auth(provider);
    
    auto prvitr = _providers.find(provider.value);
    if( prvitr == _providers.end() ) {
      _providers.emplace(provider, [&]( auto& item ) {
          item.provider = provider;
          item.approved = false;
          item.url = url;
          item.email = email;
          item.im = im;
        });
    }
    else {
      _providers.modify(*prvitr, provider, [&]( auto& item ) {
          item.url = url;
          item.email = email;
          item.im = im;
        });
    }
  }
  
  
  ACTION approveprv(name provider)
  {
    require_auth(permission_level(_self, name("admin")));

    auto prvitr = _providers.find(provider.value);
    eosio_assert( prvitr != _providers.end(), "Cannot find provider");
    _providers.modify(*prvitr, same_payer, [&]( auto& item ) {
        item.approved = true;
      });
  }

  
  ACTION updrec(name network, name type, name provider, name srvname,
                string url, string continent, string country)
  {
    require_auth(provider);
    auto netitr = _networks.find(network.value);
    eosio_assert( netitr != _networks.end(), "Invalid network name");
    auto typitr = _apitypes.find(type.value);
    eosio_assert(typitr != _apitypes.end(), "Invalid API type");
    auto prvitr = _providers.find(provider.value);
    eosio_assert( prvitr != _providers.end(), "Unknown provider name");
    eosio_assert( prvitr->approved, "This provider is not yet approved");

    bool found = false;
    for(const string &cnt : iso_continents) {
      if( continent == cnt ) {
        found = true;
        break;
      }
    }
    eosio_assert(found, "Invalid continent name");

    if( country != "ANY" ) {
      eosio_assert(country.length() == 2, "ISO country code must be 2 letters");
      for( int i = 0; i < country.length(); i++ ) {
        char c = country[i];
        eosio_assert('A' <= c && c <= 'Z', "Invalid character in ISO country code");
      }
    }

    records _records(_self, network.value);
    auto idx = _records.get_index<name("recidx")>();
    auto recitr = idx.lower_bound(recidx(type, provider));
    while( recitr != idx.end() &&
           recitr->type == type && recitr->provider == provider &&
           recitr->srvname != srvname ) {
      recitr++;
    }

    if( recitr != idx.end() &&
        recitr->type == type && recitr->provider == provider &&
        recitr->srvname == srvname ) {
      _records.modify(*recitr, provider, [&]( auto& item ) {
          item.url = url;
          item.continent = continent;
          item.country = country;
        });
    }
    else {
      _records.emplace(provider, [&]( auto& item ) {
          item.id = _records.available_primary_key();
          item.type = type;
          item.provider = provider;
          item.srvname = srvname;
          item.url = url;
          item.continent = continent;
          item.country = country;
        });
    }
  }


  
  ACTION delrec(name network, name type, name provider, name srvname)
  {
    require_auth(provider);
    records _records(_self, network.value);
    auto idx = _records.get_index<name("recidx")>();
    auto recitr = idx.lower_bound(recidx(type, provider));
    while( recitr != idx.end() &&
           recitr->type == type && recitr->provider == provider ) {
      if( recitr->srvname == srvname ) {
        _records.erase(*recitr);
        return;
      }
      recitr++;
    }
    eosio_assert(false, "Cannot find such record");
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
    networks _networks;


    struct [[eosio::table("apitypes")]] apitype {
      name         type;
      string       description;
      string       url;
      auto primary_key()const { return type.value; }
    };

    typedef eosio::multi_index<name("apitypes"), apitype> apitypes;
    apitypes _apitypes;


    struct [[eosio::table("providers")]] provider {
      name         provider;
      bool         approved;
      string       url;
      string       email;
      string       im;
      auto primary_key()const { return provider.value; }
    };

    typedef eosio::multi_index<name("providers"), provider> providers;
    providers _providers;
    

    inline static uint128_t recidx(name type, name provider) {
      return (((uint128_t)type.value)<<64) | provider.value;
    }

    // network name is the scope
    struct [[eosio::table("records")]] record {
      uint64_t     id;
      name         type;
      name         provider;
      name         srvname;
      string       url;
      string       continent;
      string       country;
      auto primary_key()const { return id; }
      uint64_t get_type() const { return type.value; }
      uint128_t get_recidx() const { return recidx(type, provider); }
    };

    typedef eosio::multi_index<name("records"), record,
      indexed_by<name("type"), const_mem_fun<record, uint64_t, &record::get_type>>,
      indexed_by<name("recidx"), const_mem_fun<record, uint128_t, &record::get_recidx>>
    > records;
    
};


EOSIO_DISPATCH(apidirectory, (setnetwork)(setapitype)(setprovider)(approveprv)(updrec)(delrec))

