#!/bin/bash

cd ~/eosio

nodeos -e -p eosio \
--plugin eosio::producer_plugin \
--plugin eosio::producer_api_plugin \
--plugin eosio::chain_api_plugin \
--plugin eosio::http_plugin \
--plugin eosio::history_plugin \
--plugin eosio::history_api_plugin \
--filter-on="*" \
--access-control-allow-origin='*' \
--contracts-console \
--http-validate-host=false \
--http-server-address=0.0.0.0:8888 \
--verbose-http-errors >> nodeos.log 2>&1 &

sleep 20

curl http://localhost:8888/v1/chain/get_info

read -p "Press enter to continue"

cat *.pwd | cleos wallet unlock

cleos create account eosio eosio.token EOS8RsvsEscy2ZttFiGyeTFeKqUGJjGS6Cor4ZW3d8TGmQYAX59qD

sleep 1

cd ../eosio.contracts/contracts/eosio.token

cleos set contract eosio.token . --abi eosio.token.abi -p eosio.token@active

sleep 1

cleos create account eosio lottery EOS8RsvsEscy2ZttFiGyeTFeKqUGJjGS6Cor4ZW3d8TGmQYAX59qD

sleep 1

cleos create account eosio tester EOS8RsvsEscy2ZttFiGyeTFeKqUGJjGS6Cor4ZW3d8TGmQYAX59qD

sleep 1

cleos push action eosio.token create '{"issuer":"lottery", "maximum_supply":"1000000000.0000 TNT"}' -p eosio.token@active

sleep 1

cleos push action eosio.token issue '[ "lottery", "10000.0000 TNT", "memo" ]' -p lottery@active

sleep 1

cleos push action eosio.token transfer '{"from":"lottery","to":"tester","quantity":"300.0000 TNT","memo":"fuck"}' -p lottery@active

cd ~/eosio/lottery

eosio-cpp -o lottery.wasm lottery.cpp --abigen

cleos set contract lottery . -p lottery@active

sleep 1

cleos push action lottery initall '["tester"]' -p tester@active

sleep 1

cleos push action eosio.token transfer '{"from":"tester","to":"lottery","quantity":"1.0000 TNT","memo":"bet:1,2,3,4,5:1"}' -p tester@active

sleep 1

cleos set account permission lottery active --add-code



