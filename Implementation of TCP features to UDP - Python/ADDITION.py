def add(x,y):
    maxlen = max(len(x), len(y))

        #Normalize lengths
    x = x.zfill(maxlen)
    y = y.zfill(maxlen)

    result = ''
    carry = 0

    for i in range(maxlen-1, -1, -1):
        r = carry
        r += 1 if x[i] == '1' else 0
        r += 1 if y[i] == '1' else 0

           

        result = ('1' if r % 2 == 1 else '0') + result
        carry = 0 if r < 2 else 1       

    if carry !=0 : result = add(result, bin(1)) #wraping carry
    return result.zfill(maxlen)
    

#This library will perform the bit addtion and will return us the added bits ,it will follow the same addtion as mentioned in the presentation slides
