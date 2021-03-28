import random


def redeem_sh_gen(f,times):
    f.write('cleos push action lottery settarget \'["1"]\' -p tester@active\n')
    print('sleep 1 \n')
    for i in range(times):
        f.write('cleos push action lottery redeem \'["tester",%d]\' -p tester@active\r\n'%i)
        f.write('\r\n')
        f.write('sleep 1\r\n')
        f.write('\r\n')

def bet_sh_gen(f,times):
    random.seed()
    for i in range(times):
        s=random.sample(range(1,40), k=5)
        string='cleos push action eosio.token transfer \'{"from":"tester","to":"lottery","quantity":"1.0000 TNT","memo":"bet:%d,%d,%d,%d,%d:1"}\' -p tester@active\r\n'%(s[0],s[1],s[2],s[3],s[4])
        f.write(string)
        f.write('\r\n')
        f.write('sleep 1\r\n')
        f.write('\r\n')
if __name__=="__main__":
    with open(r'G:\Oracle\share_floder\lottery\betloop.sh','w') as f:
        bet_sh_gen(f,50)
    with open(r'G:\Oracle\share_floder\lottery\redeemloop.sh','w') as f:
        redeem_sh_gen(f,50)

    

# sleep 1

# cleos push action lottery settarget '["1"]' -p tester@active

# sleep 1

# cleos push action lottery redeem '["tester",0]' -p tester@active