#include "lottery.hpp"
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <vector>
#include <string>

using namespace eosio;
using namespace std;

/*memo

get_first_receiver() or  getself()?


*/

void lottery::bet(eosio::name user,eosio::name to,eosio::asset quantity,std::string memo){
    if(to!=get_self())
        return;
    ticket_idx ticket_tbl(get_self(), user.value);
    ticket tmp_ticket;
    tmp_ticket.owner=user;
    check(bet_allow(quantity),"error:symbol");
    tmp_ticket.betfunds=quantity;
    string_decode(memo,&tmp_ticket);
    auto apk=ticket_tbl.available_primary_key();
    ticket_tbl.emplace(get_self(),[&](auto& row){
        row=tmp_ticket;
        row.serialID=apk;
    });
    print("your ticket ID is : ");
    print(apk);
    print("  \n Please rember it");
}
void lottery::redeem(eosio::name user,uint64_t   serialID){
    //to do check user identity
    //
    ticket_idx ticket_tbl(get_self(), user.value);
    auto ticket_itr=ticket_tbl.find(serialID);

    check(ticket_itr!=ticket_tbl.end(),"error:serialID");

    allow_idx allow_tbl(get_self(),get_self().value);
    auto allow_itr=allow_tbl.find(stoll(ticket_itr->period));

    check(allow_itr->status==0,"error:period");

    const auto user_number=ticket_itr->numbers;
    const auto dealer_numbers=allow_itr->numbers;

    double coe=0;
    int count=0;
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++){
            if(user_number.at(i)==dealer_numbers.at(j)){
                count++;
            }
        }
    }
    if(count==0 ||count==1){
        coe=0;
    }
    else if (count==2){
        coe=PRIZE_4;
    }
    else if (count==3){
        coe=PRIZE_3;
    }
    else if (count==4){
        coe=PRIZE_2;
    }
    else if (count==5){
        coe=PRIZE_1;
    }
    if(coe){
        sendtoken(user,asset(ticket_itr->betfunds.amount*coe,accept_symbol));
    }
    ticket_tbl.erase(ticket_itr);
    
}

void lottery::initall(eosio::name user){
    require_auth(name(INVESTOR));
    allow_idx allow_table(get_self(),get_self().value);

    allow_table.emplace(get_self(),[&](auto& row){
        row.period=std::stoull(std::to_string(START_PERIOD));
        row.status=1;
    });

    for(int i=1;i<10;i++){
        allow_table.emplace(get_self(),[&](auto& row){
            row.period=std::stoull(std::to_string(START_PERIOD+i));
            row.status=2;
        });
    }
    auto mixd =now();
    const char * mixedChar = reinterpret_cast<const char *>(&mixd);
    seed_idx seed_tbl(get_self(),get_self().value);
    seed_tbl.emplace(get_self(),[&](auto& row){
        row.key=1;
        row.seed=sha256((char *)mixedChar, sizeof(mixedChar));
        row.status=1;
    });


}

void lottery::shutdown(eosio::name user){
    require_auth(name(INVESTOR));

    allow_idx allow_tbl(get_self(),get_self().value);
    auto allow_itr =allow_tbl.begin();


    ticket_idx ticket_tbl(get_self(),get_self().value);
    auto ticket_itr=ticket_tbl.begin();
    auto next_ticket_itr=ticket_itr;

    


    while(ticket_itr!=ticket_tbl.end()){
        next_ticket_itr++;
        allow_itr=allow_tbl.find(stoll(ticket_itr->period));
        if(allow_itr->status==0){
            action{
                permission_level{get_self(), "active"_n},
                get_self(),
                "redeem"_n,
                std::make_tuple(get_self(),ticket_itr->period)
            }.send();
            ticket_itr=ticket_tbl.begin();
        }
        else{
            sendtoken(ticket_itr->owner,ticket_itr->betfunds);
            ticket_tbl.erase(ticket_itr);
            ticket_itr = next_ticket_itr;
        }
    }

    seed_idx seed_tbl(get_self(),get_self().value);
    seed_tbl.modify(seed_tbl.begin(),get_self(),[&](auto &row){
                row.status=0;
            });

    //transform alltoken to INVESTOR
    return;
}

void lottery::settarget(std::string period){
    //check user
    //check period
    allow_idx allow_tbl(get_self(),get_self().value);
    auto allow_itr=allow_tbl.find(stoll(period));

    check(allow_itr!=allow_tbl.end(),"error:period dosent exist");
    check(allow_itr->status==1,"error:period ");

    seed_idx seed_tbl(get_self(),get_self().value);
    auto seed_itr=seed_tbl.find(1);

    checksum256 old_seed=seed_itr->seed;
    auto mixd = old_seed.data()+now();
    const char * mixedChar = reinterpret_cast<const char *>(&mixd);
    checksum256 new_seed;
    new_seed=sha256((char *)mixedChar, sizeof(mixedChar));
    
    
    
    seed_tbl.modify(seed_itr,get_self(),[&](auto &row){
                row.seed=new_seed;
            });

    bool flag=true;
    vector <int> numbers;

    
    while(flag){
        flag=false;
        int i=0;
        auto random_num=seed_itr->seed.extract_as_byte_array()[0];
        for(i=1;i<30;i++){
            random_num+=seed_itr->seed.extract_as_byte_array()[i];
            if(i%6==5){
                numbers.push_back((((int)random_num)%39) +1);
                random_num=0;
            }
        }
        
        for(i=0;i<5;i++){
            for(int j=0;j<i;j++){
                    if(numbers.at(i)==numbers.at(j))
                        flag=true;
                }
            if(flag){
                lottery::change_seed(numbers);
                
                break;
            }
                
        }
    }
    allow_tbl.modify(allow_itr,get_self(),[&](auto &row){
                row.numbers.assign(numbers.begin(), numbers.end());
                row.status=0;
        });
    allow_itr++;
    allow_tbl.modify(allow_itr,get_self(),[&](auto &row){
                row.status=1;
        });
}


