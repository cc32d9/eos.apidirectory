# EOS API Directory smart contract

## Workflow

This is an EOS smart contract that holds a catalog of various
infrastructure APIs, such as history or account status APIs.

One directory on EOS mainnet lists API endpoints for all known
EOSIO-based networks. Network names and API types are added by directory
administrators.

Each API provider is identified by an account name. Administrators have
to approve APi providers in order to avoid spamming.

API providers add records for API endpoints that they are
maintaining. Each record has the following set of attributes:

* `network`: the blockchain name. It is used as scope name in `records`
  table.

* `type`: one of known API types from the list of types maintained by
  administrators.

* `provider`: provider's EOS account name.

* `srvname`: 12-symbol service name that helps distinguishing between
  entries belonging to the same provider. This can also be an empty
  string.

* `url`: the endpoint URL. Some API types are not following th estandard
  URL scheme, so the syntax is dependent on API type. For example, p2p
  peer would be in the form of `host:port`.

* `continent`: continent code where the API is served from. If the
  service is geographically distributed across multiple continents, use
  "ANY". Valid values are: "AF", "AN", "AS", "EU", "NA", "OC",
  "SA". "ANY".

* `country`: ISO country code of the country the API is served from. Use
  "ANY" if the URL is served from multiple countries.

* `flags`: a 32-bit unsigned integer indicating implementation options,
  such as flavors of history API. Each API type defines its own rules
  for this field.

* `revision`: a 32-bit integer that is automatically increased every
  time the provider updates the record.

* `updated_on`: timestamp of latest revision.

* `audited_by`: EOS account name of the auditor.

* `audit_ipfs_file`: IPFS file hash with the audit report.

* `audited_on`: timestamp of the audit.


The API provider may modify or delete a record in the directory by
specifying its `network`, `type`, `provider`, and `srvname`
attributes. The `updrec` action either adds a new record, or modifies an
existing one.


Each record may be audited by an independent auditor. Auditors are
selected by administrators, and each of them has a PGP key publiushed on
one of PGP key servers.

Once the auditor verifies that the API endpoint ownership and validity,
he or she publishes an audit report in a file on IPFS. The format of the
audit report is yet to be formalized. Once the report is published, the
auditor sends `audited` action to the directory, and the corresponding
record is marked as audited.

An auditor cannot overwrite another auditor's report. Existing reports
by the same auditor can be updated.

An auditor may also revoke the report if the service is no longer
satisfying the requirements.

Once the provider updates a record, its audit report is cleared, and the
auditor would have to re-evaluate it again.


## Api types and flags

### `v1chain`

This is standard HTTP API provided by `chain_plugin` in `nodeos`. It
provides account information, contract table contents, and allows
pushing transactions to the network.


### `v1history`

`v1history` API was originally implemented in `history_plugin` in
`nodeos`, but it appeared not to be scalable enough. There are several
implementations that emulate the history API via other databases.

The lower 5 bits of the `flags` are reserved for implementation details,
such as:

* 0: original `history_api`

* 1: JavaScript adapter on top of MongoDB database by Cryptolions:
  https://github.com/CryptoLions/EOS-mongo-history-API

* 2: `elasticsearch_plugin` and Python API layer:
https://github.com/EOSLaoMao/elasticsearch_plugin
https://github.com/EOSTribe/ESHistoryAPI

* 3: Light History API plugin made by Greymass:
  https://github.com/greymass/eos/tree/hapi-limited/



### `p2p`

This is peer-to-peer protocol used by `nodeos` servers for exchanging
the blockchain data and synchronizing against each other.

### `hyperion`

Hyperion is a new-generation history API developed by EOS Rio:
https://github.com/eosrio/Hyperion-History-API

### `lightapi`

Light API is providing current token balances and mapping of keys to
accounts: https://github.com/cc32d9/eos_zmq_light_api

### `dfuse`

dfuse is a proprietary streaming API provided by EOS Canada.




## Administrator actions

```
# easy alias for switching between testnet and mainnet

#alias cl='cleos -v -u http://jungle2.cryptolions.io'
alias cl='cleos -v -u https://api.eossweden.se'

# "admin" privilege is required for administrator actions. It can be
# assigned to a group of administrator accounts.

cl set account permission apidirectory admin \
 '{"threshold": 1,"keys": [],"accounts": [{"permission": {"actor": "cc32dninexxx","permission": "active"},"weight": 1}],"waits": []}' \
 owner -p apidirectory@owner
cl set action permission apidirectory apidirectory setnetwork admin
cl set action permission apidirectory apidirectory setapitype admin
cl set action permission apidirectory apidirectory approveprv admin
cl set action permission apidirectory apidirectory setauditor admin

# Register all known EOSIO based blockchains

cl push action apidirectory setnetwork '["eos", "aca376f206b8fc25a6ed44dbdc66547c36c6c33e3a119ffbeaef943642f0e906", "EOS Mainnet", ""]' -p apidirectory@admin

cl push action apidirectory setnetwork '["jungle", "e70aaab8997e1dfce58fbfac80cbbb8fecec7b99cf982a9444273cbc64c41473", "EOS Jungle testnet", "https://jungletestnet.io/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["kylin", "5fff1dae8dc8e2fc4d5b23b2c7665c97f9e9d8edf2b6485a86ba311c25639191", "EOS Kylin testnet", "https://www.cryptokylin.io/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["worbli", "73647cde120091e0a4b85bced2f3cfdb3041e266cbbe95cee59b73235a1b3b6f", "Worbli", "https://worbli.io/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["bos", "d5a3d18fbb3c084e3b1f3fa98c21014b5f3db536cc15d08f9f6479517c6a3d86", "BOSCORE", "https://boscore.io/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["telos", "4667b205c6838ef70ff7988f6e8257e8be0e1284a2f59699054a018f743b1d11", "Telos", "https://telosfoundation.io/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["meet.one", "cfe6486a83bad4962f232d48003b1824ab5665c36778141034d75e57b956e422", "MEET.ONE", "https://meet.one/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["eosforce", "bd61ae3a031e8ef2f97ee3b0e62776d6d30d4833c8f7c1645c657b149151004b", "EOS Force", "https://www.eosforce.io/"]' -p apidirectory@admin


# Register all known API types

cl push action apidirectory setapitype '["v1chain", "chain_plugin in nodeos", "https://github.com/EOSIO/eos/tree/master/plugins/chain_plugin"]' -p apidirectory@admin

cl push action apidirectory setapitype '["v1history", "history_plugin and its emulations", "https://github.com/EOSIO/eos/tree/master/plugins/history_plugin"]' -p apidirectory@admin

cl push action apidirectory setapitype '["p2p", "net_plugin in nodeos", "https://github.com/EOSIO/eos/tree/master/plugins/net_plugin"]' -p apidirectory@admin

cl push action apidirectory setapitype '["hyperion", "Scalable Full History API Solution", "https://github.com/eosrio/Hyperion-History-API"]' -p apidirectory@admin

cl push action apidirectory setapitype '["lightapi", "EOS Light API", "https://github.com/cc32d9/eos_zmq_light_api"]' -p apidirectory@admin

cl push action apidirectory setapitype '["dfuse", "dfuse Streaming API for EOS", "https://www.dfuse.io/"]' -p apidirectory@admin


# Register audiors

cl push action apidirectory setauditor '["rqeofihcqeco", "Rqeofi Hcqeco", "A999 B749 8D1A 8DC4 73E5 3C92 309F 635D AD1B 5517", "https://pgp.mit.edu/", "rqeofi@hcqeco.com", "telegram:rqeofihcqeco"]'  -p apidirectory@admin

```


# API Provider actions

```
#alias cl='cleos -v -u http://jungle2.cryptolions.io'
alias cl='cleos -v -u https://api.eossweden.se'

# API provider registers themselves

cl push action apidirectory setprovider '["cc32dninexxx", "https://github.com/cc32d9", "cc32d9@gmail.com", "telegram:cc32d9"]' -p cc32dninexxx@active

# Administrator has to approve the new provider

cl push action apidirectory approveprv '["cc32dninexxx"]' -p apidirectory@admin

# Provider registers all their API endpoints. If they support multiple
# endpoints of the same type on the same network, use different service names
# to distinguish the records.

cl push action apidirectory updrec '["eos", "lightapi", "cc32dninexxx", "worldwide", "https://api.net.light.xeos.me", "ANY", "ANY", 0]' -p cc32dninexxx@active

```


# Auditor actions

```
# Submit an audit
cl push action apidirectory audited '["rqeofihcqeco", "eos", "lightapi", "cc32dninexxx", "worldwide", "QmQgSzNmb5pXd1XpzvLdKBBAdiTrqXcJHrTX5RQhHyWRTd"]' -p rqeofihcqeco@active


# Revoke an audit
cl push action apidirectory revokeaudit '["rqeofihcqeco", "eos", "lightapi", "cc32dninexxx", "worldwide"]' -p rqeofihcqeco@active

```


# Accessing the records

Use network name as scope in "records" table. The index #2 is by API type:

```
cl get table apidirectory eos records --index 2 --key-type name --lower lightapi

```



## Copyright and License

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
