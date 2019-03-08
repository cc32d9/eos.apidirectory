# EOS API Directory smart contract

This is an EOS smart contract that holds a catalog of various
infrastructure APIs, such as history or account status APIs.

The directory lists all known EOS networks. Network names and API types
are added by directory administrators.

Each API provider is identified by an account name. Administrators have
to approve APi providers in order to avoid spamming.

Each APi provider is allowed to specify one API URL of each type for
each network. Apart from API URL, the provider needs to specify a
two-letter ISO continent code and 2-letter country code. The code "ANY"
can be used for both to indicate a georedundant API that is available
from multiple continents.

Valid continent codes: "AF", "AN", "AS", "EU", "NA", "OC", "SA".

The API provider may modify or delete a corresponding entry in the
directory.



## Administrator actions

```
alias cl='cleos -v -u http://jungle2.cryptolions.io'

cl set account permission apidirectory admin \
 '{"threshold": 1,"keys": [],"accounts": [{"permission": {"actor": "cc32dninexxx","permission": "active"},"weight": 1}],"waits": []}' \
 owner -p apidirectory@owner
cl set action permission apidirectory apidirectory setnetwork admin
cl set action permission apidirectory apidirectory setapitype admin
cl set action permission apidirectory apidirectory approveprv admin


cl push action apidirectory setnetwork '["eos", "aca376f206b8fc25a6ed44dbdc66547c36c6c33e3a119ffbeaef943642f0e906", "EOS Mainnet", ""]' -p apidirectory@admin

cl push action apidirectory setnetwork '["jungle", "e70aaab8997e1dfce58fbfac80cbbb8fecec7b99cf982a9444273cbc64c41473", "EOS Jungle testnet", "https://jungletestnet.io/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["kylin", "5fff1dae8dc8e2fc4d5b23b2c7665c97f9e9d8edf2b6485a86ba311c25639191", "EOS Kylin testnet", "https://www.cryptokylin.io/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["worbli", "73647cde120091e0a4b85bced2f3cfdb3041e266cbbe95cee59b73235a1b3b6f", "Worbli", "https://worbli.io/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["bos", "d5a3d18fbb3c084e3b1f3fa98c21014b5f3db536cc15d08f9f6479517c6a3d86", "BOSCORE", "https://boscore.io/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["telos", "4667b205c6838ef70ff7988f6e8257e8be0e1284a2f59699054a018f743b1d11", "Telos", "https://telosfoundation.io/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["meet.one", "cfe6486a83bad4962f232d48003b1824ab5665c36778141034d75e57b956e422", "MEET.ONE", "https://meet.one/"]' -p apidirectory@admin

cl push action apidirectory setnetwork '["eosforce", "bd61ae3a031e8ef2f97ee3b0e62776d6d30d4833c8f7c1645c657b149151004b", "EOS Force", "https://www.eosforce.io/"]' -p apidirectory@admin



cl push action apidirectory setapitype '["v1chainpl", "chain_plugin in nodeos", "https://github.com/EOSIO/eos/tree/master/plugins/chain_plugin"]' -p apidirectory@admin

cl push action apidirectory setapitype '["v1historypl", "Original history_plugin in nodeos", "https://github.com/EOSIO/eos/tree/master/plugins/history_plugin"]' -p apidirectory@admin

cl push action apidirectory setapitype '["nodeosp2p", "net_plugin in nodeos", "https://github.com/EOSIO/eos/tree/master/plugins/net_plugin"]' -p apidirectory@admin

cl push action apidirectory setapitype '["hyperion", "Scalable Full History API Solution", "https://github.com/eosrio/Hyperion-History-API"]' -p apidirectory@admin

cl push action apidirectory setapitype '["lightapi", "EOS Light API", "https://github.com/cc32d9/eos_zmq_light_api"]' -p apidirectory@admin

cl push action apidirectory setapitype '["dfuse", "dfuse Streaming API for EOS", "https://www.dfuse.io/"]' -p apidirectory@admin

```


# API Provider actions

```
alias cl='cleos -v -u http://jungle2.cryptolions.io'

cl push action apidirectory setprovider '["cc32dninexxx", "https://github.com/cc32d9", "cc32d9@gmail.com", "telegram:cc32d9"]' -p cc32dninexxx@active

## Administrator has to approve the new provider
cl push action apidirectory approveprv '["cc32dninexxx"]' -p apidirectory@admin


cl push action apidirectory updrec '["eos", "lightapi", "cc32dninexxx", "https://api.net.light.xeos.me", "ANY", "ANY"]' -p cc32dninexxx@active

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
