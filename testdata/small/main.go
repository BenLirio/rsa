package main

import (
    "math/big"
    "encoding/binary"
    "os"
)

type Numbers [2]string

var Cases []Numbers = []Numbers{
    Numbers{"111111", "1"},
    Numbers{"1", "fffffff"},
    Numbers{"ffffffffffffff", "ffffffffffffff"},
    Numbers{"ffffffffffffffffffffffffffffffffffffffffffffffff", "ffffffffffffffffffffffffffffffffffffffffffffffff"},
    Numbers{"111111111111111111111111111111111111111111111111", "1"},
    Numbers{"111111111111111111111111111111111111111111111111", "111111111111111111111111111111111111111111111111"},
    Numbers{"111111111111111111111111111111111111111111111111", "2"},
    Numbers{"212121212121212121212121212121212121212121212121", "2"},
    Numbers{"212121212121212121212121212121212121212121212121", "111111111111111111111111111111111111111111111111"},
    Numbers{"111111111111111111111111111111111111111111111112", "111111111111111111111111111111111111111111111112"},
}

func rev(b []byte) []byte {
    for i := 0; i < len(b)/2; i++ {
        b[i], b[len(b)-i-1] = b[len(b)-i-1], b[i]
    }
    return b
}

func main() {
    fp, err := os.Create("numbers")
    if err != nil { panic(err) }
    for i := 0; i < len(Cases); i++ {
        a, ok := new(big.Int).SetString(Cases[i][0], 16)
        if ok != true { panic("SetString Failed") }
        err = binary.Write(fp, binary.LittleEndian, uint32(len(a.Bytes())))
        if err != nil { panic(err) }
        fp.Write(rev(a.Bytes()))


        b, ok := new(big.Int).SetString(Cases[i][1], 16)
        if ok != true { panic("SetString Failed") }
        err = binary.Write(fp, binary.LittleEndian, uint32(len(b.Bytes())))
        if err != nil { panic(err) }
        fp.Write(rev(b.Bytes()))

        c := new(big.Int).Mul(a, b)
        err = binary.Write(fp, binary.LittleEndian, uint32(len(c.Bytes())))
        if err != nil { panic(err) }
        fp.Write(rev(c.Bytes()))

    }
    fp.Close()
}
