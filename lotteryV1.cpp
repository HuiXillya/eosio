#include <eosio/eosio.hpp>
using namespace eosio;

class [[eosio::contract("lottery")]] lottery : public eosio::contract {

    public:
        //constructor
        lottery(name receiver, name code,  datastream<const char*> ds):contract(receiver, code, ds) {}

        [[eosio::action]]//add a bet
        void bet(name adder,std::string p,unsigned char n1,unsigned char n2,unsigned char n3,unsigned char n4,unsigned char n5){
            //todo cheack
            single_bet_index betlist(get_first_receiver(), get_first_receiver().value);
            //send payer and lambda
            betlist.emplace(adder,[&](auto& row){
                row.serialID=betlist.available_primary_key();
                row.accountname=adder;
                row.num1=n1;
                row.num1=n2;
                row.num1=n3;
                row.num1=n4;
                row.num1=n5;
                for (int i=0;i<9;i++)
                    row.periodnum[i]=p[i];
            });
        }
        [[eosio::action]]
        void settarget(uint64_t p,unsigned char n1,unsigned char n2,unsigned char n3,unsigned char n4,unsigned char n5){
            require_auth(get_self());
            target_index targetlist(get_first_receiver(), get_first_receiver().value);
            targetlist.emplace(get_self(),[&](auto& row){
                row.periodnum=p;
                row.num1=n1;
                row.num1=n2;
                row.num1=n3;
                row.num1=n4;
                row.num1=n5;

            });    
        }
        
    private:
        struct [[eosio::table]] single_bet {
            uint64_t   serialID;
            name accountname;
            std::string periodnum;
            unsigned char num1 ;
            unsigned char num2 ;
            unsigned char num3 ;
            unsigned char num4 ;
            unsigned char num5 ;
            uint64_t primary_key() const { return serialID; }
            uint64_t searchforname() const { return accountname.value; }
        };
        struct [[eosio::table]] target{
            uint64_t periodnum;
            unsigned char num1 ;
            unsigned char num2 ;
            unsigned char num3 ;
            unsigned char num4 ;
            unsigned char num5 ;
            uint64_t primary_key() const { return periodnum; }  
        };
        



    typedef  eosio::multi_index<"betlist"_n, single_bet,
                indexed_by<"byname"_n, const_mem_fun<single_bet,uint64_t, &single_bet::searchforname>>> single_bet_index;
    typedef eosio::multi_index<"targetlist"_n,target> target_index;
};
