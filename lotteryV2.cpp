#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#define ERR_BOUND 40


using namespace eosio;

class [[eosio::contract("lottery")]] lottery : public eosio::contract {

    public:
        //constructor
        lottery(name receiver, name code,  datastream<const char*> ds):contract(receiver, code, ds),lottery_symbol("SYS",4){
            //lottery_symbol=lottery_symbol.symbol("SYS",4);
        }

        [[eosio::action]]//add a bet
        void bet(name beter,name to,std::string p, eosio::asset quantity,unsigned char n1,unsigned char n2,unsigned char n3,unsigned char n4,unsigned char n5){
            //cheack all condition
            check_bet(beter,to,p,quantity,n1,n2,n3,n4,n5);
            single_bet_index betlist(get_first_receiver(), beter.value);
            //send payer and lambda
            betlist.emplace(beter,[&](auto& row){
                row.serialID=betlist.available_primary_key();
                row.accountname=beter;
                row.betfunds = quantity;
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
            print("%d",p);
            target_index targetlist(get_first_receiver(), get_first_receiver().value);
            auto last_itr = targetlist.find(p);
            if (last_itr != targetlist.end()){
                targetlist.modify(last_itr,get_self(),[&](auto& row){
                    row.num1=n1;
                    row.num1=n2;
                    row.num1=n3;
                    row.num1=n4;
                    row.num1=n5;
            });
            } 
            
            targetlist.emplace(get_self(),[&](auto& row){
                row.periodnum=p+1;
                row.num1=ERR_BOUND;
                row.num1=ERR_BOUND;
                row.num1=ERR_BOUND;
                row.num1=ERR_BOUND;
                row.num1=ERR_BOUND;

            });    
        }
        [[eosio::on_notify("eosio.token::transfer")]]
        void deposit(name hodler, name to, eosio::asset quantity, std::string memo){
          if (to != get_self() || hodler == get_self()){
            print("These are not the contract you are looking for.");
            return;
          }
        
          check(now() < the_party, "You're way late");
          check(quantity.amount > 0, "The ammount is less the  0 ");
          
          check(quantity.symbol == lottery_symbol, "These are not the droids you are looking for.");
        
          balance_table balance(get_self(), hodler.value);
          auto hodl_it = balance.find(lottery_symbol.raw());
        
          if (hodl_it != balance.end())
            balance.modify(hodl_it, get_self(), [&](auto &row) {
              row.funds += quantity;
            });
          else
            balance.emplace(get_self(), [&](auto &row) {
              row.funds = quantity;
            });
        }
        
    private:
        struct [[eosio::table]] single_bet {
            uint64_t   serialID;
            eosio::asset betfunds;
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
        struct [[eosio::table]] balance{
            eosio::asset funds;
            uint64_t primary_key() const { return funds.symbol.raw(); }
        };
        static const uint32_t the_party = 1645525342;
        const symbol lottery_symbol;
        
        uint32_t now() {
            return current_time_point().sec_since_epoch();
        }
        void check_bet(name beter,name to,std::string p,eosio::asset quantity,unsigned char n1,unsigned char n2,unsigned char n3,unsigned char n4,unsigned char n5){
            if (to != get_self() || beter== get_self()){
                print("These are not the contract you are looking for.");
                return;
            }
            check(now() < the_party, "You're way late");
            check(quantity.amount > 0, "The ammount is less the  0 ");
            //quantity.symbol.print();
            //lottery_symbol.print();
            check(quantity.symbol == lottery_symbol, "These are not ");
            target_index targetlist(get_first_receiver(), get_first_receiver().value);
            auto itr=targetlist.find(std::stoull(p));
            print(std::stoull(p));
            check(itr !=targetlist.end(),"unsuport period  1");
            check(itr->num1==ERR_BOUND,"unsuport period  2");
            //todo 
            
        }

    

    typedef  eosio::multi_index<"betlist"_n, single_bet,
                indexed_by<"byname"_n, const_mem_fun<single_bet,uint64_t, &single_bet::searchforname>>> single_bet_index;
    typedef eosio::multi_index<"targetlist"_n,target> target_index;

    typedef eosio::multi_index<"balance"_n, balance> balance_table;
};
