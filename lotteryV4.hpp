#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <vector>
#define ERR_BOUND 40
#define BET_UPPER_BOUND 100
#define SYMBOL "TNT"
#define PRECISION 4
#define INVESTOR "invtest"
#define START_PERIOD 1

#define PRIZE_1 160000
#define PRIZE_2 4000
#define PRIZE_3 6
#define PRIZE_4 0.5

using namespace eosio;

class [[eosio::contract("lottery")]] lottery : public eosio::contract {


    
    private:

        const symbol accept_symbol;

        struct [[eosio::table]] ticket {
            uint64_t   serialID;
            eosio::name owner;
            eosio::asset betfunds;
            std::string period;
            std::vector <unsigned char> numbers;
            bool redeemed=false;
            uint64_t primary_key() const { return serialID; }
            uint64_t search_for_name() const { return owner.value; }
            uint64_t search_for_period() const { return std::stoll(period); }
        };
        struct [[eosio::table]] balance {
            eosio::name owner;
            eosio::asset funds;
            uint64_t primary_key() const { return owner.value; }
        };
        struct [[eosio::table]] target{
            uint64_t period;
            std::vector <unsigned char> numbers;
            uint64_t primary_key() const { return period; }
        };

        struct [[eosio::table]]allow{
            uint64_t period;
            unsigned char status;
            //status 0=opened 1=next open 2=unopen but allow to bet  
            uint64_t primary_key() const { return period; }
        }
        
        inline void send_summary(name user, std::string message) {
            print(message);
        };

        inline double Prize_Magnification(int bet_n,int hit_n){
            if(bet_n==5){
                if(hit_n==5)
                    return PRIZE_1;
                if(hit_n==4)
                    return PRIZE_2;
                if(hit_n==3)
                    return PRIZE_3;
                if(hit_n==2)
                    return PRIZE_4;
            }
            check(false,"unkonw error");
            return 0;
        };

        inline bool period_allow(std::string period){
            return true;
        }

        void sendtoken(name to ,asset quantity){
             action{
        permission_level{get_self(), "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        std::make_tuple(get_self(),to, quantity, std::string("witdrawal susses"))
            }.send();  
        }

    public :
        lottery( name receiver, name code, datastream<const char*> ds ):contract(receiver, code, ds),accept_symbol(SYMBOL,PRECISION){}
        //action for buyer
        [[eosio::action]]
        void bet(eosio::name user, std::string period, eosio::asset quantity,const std::vector <unsigned char> &numbers);
        [[eosio::action]]
        void redeem(eosio::name user,uint64_t   serialID);
        [[eosio::on_notify("eosio.token::transfer")]]
        void deposit(eosio::name depositer,eosio::name to,eosio::asset quantity,std::string memo);
        [[eosio::action]]
        void witdrawal(eosio::name witdrawaler,eosio::asset quantity);
    
        //temp
        [[eosio::action]]
        void takealook(eosio::name user);

        

        //action for host 
        [[eosio::action]]
        void settarget(eosio::name setter,std::string period,std::vector <unsigned char> &numbers);
        [[eosio::action]]
        void initall();
        [[eosio::action]]
        void sutdown();
    typedef  eosio::multi_index<"ticket"_n, ticket,
            indexed_by<"byname"_n, const_mem_fun<ticket,uint64_t, &ticket::search_for_name>>,
            indexed_by<"byperiod"_n, const_mem_fun<ticket,uint64_t, &ticket::search_for_period>>> ticket_index;
    typedef eosio::multi_index<"balance"_n, balance> balance_index;
    
    typedef eosio::multi_index<"target"_n, target> target_index;
    typedef eosio::multi_index<"allow"_n, allow> allow_index;
    
    };