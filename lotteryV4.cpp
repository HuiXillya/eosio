#include "lotteryV5.hpp"
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <vector>

void lottery::bet(eosio::name user, std::string period, eosio::asset quantity,const std::vector <unsigned char> &numbers){
    require_auth(user);
    ticket_index ticket_table(get_first_receiver(), get_first_receiver().value);
    balance_index balance_table(get_first_receiver(), get_first_receiver().value);
    auto balance_itr = balance_table.find(user.value);
    //------------start check---------------
    check(period_allow(period),"error:period");
    check(balance_itr!=balance_table.end(),"please deposit some money to regist!");
    check(quantity.amount > 0, "The tocken is less the  0 ");
    check(quantity.amount < BET_UPPER_BOUND, "The tocken is exceed the upper bound ");
    check(balance_itr->funds.symbol==quantity.symbol,"error:token type");
    check(balance_itr->funds.amount<=quantity.amount,"not enough money");
    check(numbers.size()==5,"unsuported size");
    bool flag=true;
    bool flag2=true
    for(int i=0;i<numbers.size()-1;i++){
        for(int j=i+1;j<numbers.size();j++){
            if(numbers.at(i)==numbers.at(j)){
                flag=false;
            }

        }
    }
    for(int i=0;i<numbers.size();i++){
        if(numbers.at(i)>=ERR_BOUND || numbers.at(i)<=0)
            flag2=false;
    }
    check(flag2."unsupport numbers")
    check(flag,"same numbers");
    //------------check end-----------------
    balance_table.modify(balance_itr,user,[&](auto& row){
        row.funds.amount-=quantity.amount;
    });
    auto apk=ticket_table.available_primary_key();
    ticket_table.emplace(user,[&](auto& row){
        row.serialID=apk;
        row.owner=user;
        row.betfunds=quantity;
        row.period=period;
        row.numbers.assign(numbers.begin(),numbers.end());
    });
    lottery::send_summary(user,"your tickit numbes is : "+std::to_string(apk));
    
};
void lottery::redeem(eosio::name user,uint64_t   serialID){
    require_auth(user);
    ticket_index ticket_table(get_first_receiver(), get_first_receiver().value);
    auto ticket_itr=ticket_table.find(serialID);
    //--------------check start--------------
    check(ticket_itr != ticket_table.end(),"error:serialID");
    check(ticket_itr->owner == user ,"error:serialID" );
    check(! ticket_itr->redeemed,"error:redeemed");
    //--------------finish check--------------
    target_index target_table(get_first_receiver(), get_first_receiver().value);
    auto target_itr =target_table.find(std::stoll(ticket_itr->period));
    balance_index balance_table(get_self(),user.value);
    auto balance_itr=balance_table.find(ticket_itr->owner.value);
    //--------------check--------------------
    check(target_itr!=target_table.end(),"error:period");
    check(target_itr->numbers.at(0) !=ERR_BOUND,"error:period");

    check(balance_itr != balance_table.end(),"error");
    //--------------finish check--------------

    //if vec.size=5
    auto vec=ticket_itr->numbers;
    int count=0;
    for(int i=0;i<4;i++){
        for(int j=i+1;j<5;j++){
            if(vec.at(i)==target_itr->numbers.at(j))
                count++;
        }
    }
    balance_table.modify(balance_itr, get_self(), [&](auto &row) {
          row.funds.amount+=ticket_itr->betfunds.amount*Prize_Magnification(5,count);
        });

    return ;
}
void lottery::deposit(eosio::name depositer,eosio::name to,eosio::asset quantity,std::string memo){
    require_auth(depositer);
    if(to !=get_self())
        return;
    //------------start check---------------
    check(quantity.amount >0,"error: amount less than 0");
    check(quantity.symbol == lottery::accept_symbol, "error:symbol wrong ");
    //------------finish check---------------
    balance_index balance_table(get_self(),depositer.value);
    auto balance_itr =balance_table.find(depositer.value);
    if(balance_itr != balance_table.end()){
        balance_table.modify(balance_itr, get_self(), [&](auto &row) {
          row.funds += quantity;
        });
        send_summary(depositer,"sussces");
    }
    else{
        balance_table.emplace(get_self(), [&](auto &row) {
          row.owner = depositer;
          row.funds = quantity;
        });
        send_summary(depositer,"sussces");
    }
};
void lottery::witdrawal(eosio::name witdrawaler,eosio::asset quantity){
    require_auth(witdrawaler);
    balance_index balance_table(get_self(),witdrawaler.value);
    auto balance_itr =balance_table.find(witdrawaler.value);
    //
    check(balance_itr != balance_table.end(),"error");
    check(quantity.amount >0,"error: amount less than 0");
    check(quantity.symbol == lottery::accept_symbol, "error:symbol wrong ");
    //
    if(quantity.amount >balance_itr->funds.amount){
        action{
        permission_level{get_self(), "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        std::make_tuple(get_self(),witdrawaler, balance_itr->funds, std::string("not enough money to witdrawal"))
      }.send();
    }
    else{
         action{
        permission_level{get_self(), "active"_n},
        "eosio.token"_n,
        "transfer"_n,
        std::make_tuple(get_self(),witdrawaler, quantity, std::string("witdrawal susses"))
      }.send();
    }

    return;
};
void lottery::takealook(eosio::name user){
    balance_index balance_table(get_self(),user.value);

    auto b_itr = balance_table.find(user.value);
    check(b_itr!=balance_table.end(),"dfhuaiuegsfhuisahuifaihu");
    b_itr->funds.print();
    
}
void lottery::settarget(eosio::name setter,std::string period,std::vector <unsigned char> &numbers){
    require_auth(setter);
    //require_auth(name(INVESTOR));
    allow_index allow_table(get_self(),s
    
    etter.value);
    auto a_itr =allow_table.find(std::stoull(period));
    //
    check(a_itr!=allow_table.end(),"error:period (usupport)")
    check(a_itr->status==1,"error:period (next)");
    //
    target_table.emplace(setter,[&](auto& row){
        row.period=std::stoull(period);
        row.numbers.assign(numbers.begin(),numbers.end());
    });
    a_itr++;
    if(a_itr!=allow_table.end()){
        allow_table.modify(a_itr,setter,[&](auto& row){
            row.status=1
        })
    }
}
void lottery::initall(eosio::name setter){
    require_auth(setter);
    //require_auth(name(INVESTOR));
    allow_index allow_table(get_self(),setter.value);

    allow_table.emplace(setter,[&](auto& row){
        row.period=std::stoull(std::to_string(START_PERIOD));
        row.status=1;
    });
    for(int i=1;i<10;i++){
        allow_table.emplace(setter,[&](auto& row){
            row.period=std::stoull(std::to_string(START_PERIOD+i));
            row.status=2;
    });

}
void lottery::sutdown(){



    return;
}