#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <vector>

#define ERR_BOUND 40
#define BET_UPPER_BOUND 100
#define SYMBOL "TNT"
#define PRECISION 4
#define INVESTOR "tester"
#define SELFNAME "lotery"
#define START_PERIOD 1

#define PRIZE_1 160000
#define PRIZE_2 4000
#define PRIZE_3 6
#define PRIZE_4 0.5


using namespace eosio;
using namespace std;

class [[eosio::contract("lottery")]] lottery : public eosio::contract {
    private:
        const symbol accept_symbol;

        struct [[eosio::table]] ticket {
            uint64_t   serialID;
            eosio::name owner;
            eosio::asset betfunds;
            std::string period;
            std::vector <int> numbers;
            uint64_t primary_key() const { return serialID; }
            uint64_t search_for_name() const { return owner.value; }
            uint64_t search_for_period() const { return std::stoll(period); }
        };
        struct [[eosio::table]]allow{
            uint64_t period;
            uint32_t time;
            std::vector <int> numbers;
            unsigned char status;
            //status 0=opened, 1=next open, 2=unopen but allow to bet  
            uint64_t primary_key() const { return period; }
        };
        struct seed{
            uint64_t key;
            
            checksum256 seed;
            uint8_t status;
            //status 1 = accept, 0 = reject
            uint64_t primary_key() const { return key; }
        };

        struct [[eosio::table]] target{
            uint64_t period;
            std::vector <unsigned char> numbers;
            uint64_t primary_key() const { return period; }
        };
        


        inline bool period_allow(std::string period,ticket *r_ticket){
            return true;
        }
        inline bool bet_allow(eosio::asset quantity){
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
        inline uint32_t now() {
             return current_time_point().sec_since_epoch();
        }
        
        void change_seed(const std::vector <int> numbers){
            seed_idx seed_tbl(get_self(),get_self().value);
            auto seed_itr=seed_tbl.find(1);

            checksum256 old_seed=seed_itr->seed;
            auto mixd = old_seed.data()+now()+numbers.at(0)+numbers.at(1)+numbers.at(2)+numbers.at(3)+numbers.at(4);
            const char *mixedChar = reinterpret_cast<const char *>(&mixd);
            checksum256 new_seed;
            new_seed=sha256((char *)mixedChar, sizeof(mixedChar));

            seed_tbl.modify(seed_itr,get_self(),[&](auto &row){
                row.seed=new_seed;
            });

        };
        void split(const string& s, char c,vector<string>& v) {
            string::size_type i = 0;
            string::size_type j = s.find(c);

            while (j != string::npos) {
                v.push_back(s.substr(i, j-i));
                i = ++j;
                j = s.find(c, j);

                if (j == string::npos)
                    v.push_back(s.substr(i, s.length()));
            }
        }
        void string_decode(std::string memo,ticket * r_ticket){
            memo.erase(memo.begin(), find_if(memo.begin(), memo.end(), [](int ch) {
                    return !isspace(ch);
                }));
            memo.erase(find_if(memo.rbegin(), memo.rend(), [](int ch) {
                    return !isspace(ch);
            }).base(), memo.end());

            vector<string> v;
            split(memo, ':', v);
            check(v.size() == 3, "error:memo");
            //bet:xxxxx....
            check(v.at(0)=="bet","error:memo ,with out bet");
            //numbers
            r_ticket->numbers=string_numbers(v.at(1));
            //period
            check(period_allow(v.at(2),r_ticket),"error:period");
            r_ticket->period=v.at(2);
        }
        inline vector<int> string_numbers(string n_s){
            vector<string> s_v;
            split(n_s, ',', s_v);
            check(s_v.size()==5,"error:number of numbrers");
            vector<int> r_v;
            for(int i=0;i<5;i++){
                int t=stoi(s_v.at(i));
                for(int j=0;j<i;j++){
                    check(t!=r_v.at(j),"error:same number");
                }
                check(t<=39,"error:number range");
                check(t>=1,"error:number range");
                r_v.push_back(t);
            }
            return r_v;
        }
       
         
    public :
        lottery( name receiver, name code, datastream<const char*> ds ):contract(receiver, code, ds),accept_symbol(SYMBOL,PRECISION){}
        //action for buyer

        [[eosio::on_notify("eosio.token::transfer")]]
        void bet(eosio::name user,eosio::name to,eosio::asset quantity,std::string memo);
        [[eosio::action]]
        void redeem(eosio::name user,uint64_t   serialID);

        //action for host 
        [[eosio::action]]
        void settarget(std::string period);
        [[eosio::action]]
        void initall(eosio::name user);
        [[eosio::action]]
        void shutdown(eosio::name user);
    typedef  eosio::multi_index<"ticket"_n, ticket,
            indexed_by<"byname"_n, const_mem_fun<ticket,uint64_t, &ticket::search_for_name>>,
            indexed_by<"byperiod"_n, const_mem_fun<ticket,uint64_t, &ticket::search_for_period>>> ticket_idx;
    
    typedef eosio::multi_index<"target"_n, target> target_idx;
    typedef eosio::multi_index<"allow"_n, allow> allow_idx;
    typedef eosio::multi_index<"seed"_n, seed> seed_idx;
    
    };