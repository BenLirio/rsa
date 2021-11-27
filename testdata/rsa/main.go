package main

import (
    "crypto/rsa"
    "fmt"
    "math/big"
    "crypto/rand"
    "os"
)


func rev(bs []byte) []byte {
    for i := 0; i < len(bs)/2; i++ {
        bs[i], bs[len(bs)-i-1] = bs[len(bs)-i-1], bs[i]
    }
    return bs
}

func write(key string, val *big.Int) {
    fp, err := os.Create(key)
    if err != nil { panic(err) }
    fp.Write(rev(val.Bytes()))
    buffLen := 2048>>3 - len(val.Bytes())
    if buffLen < 0 {
        fmt.Println("Recieved value with more than 2048 bits")
    }
    buff := make([]byte, buffLen)
    fp.Write(buff)
    fp.Close()
}

func main() {
    key, err := rsa.GenerateKey(rand.Reader, 2048)
    if err != nil { panic(err) }

    N := key.PublicKey.N
    E := new(big.Int).SetUint64(uint64(key.PublicKey.E))
    D := key.D
    M := new(big.Int).SetUint64(uint64(42))
    C := new(big.Int).Exp(M, E, N)
    Mp := new(big.Int).Exp(C, D, N)

    // Write out the prime and modulo
    write("M", M)
    write("E", E)
    write("N", N)
    write("C", C)
    write("D", D)
    if Mp.Cmp(M) != 0 {
        fmt.Println("Failed")
    }
    for i := 2; i < 9; i++ {
        M.Mul(M, M)
        write(fmt.Sprintf("M_%d", i), M)
    }
}
