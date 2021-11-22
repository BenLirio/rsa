package main

import (
    "math/big"
    "encoding/binary"
    "os"
)

type Numbers [2]string

var Cases []Numbers = []Numbers{
    Numbers{"ffff", "1"},
    Numbers{"ffff", "1"},
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
        a, _ := new(big.Int).SetString(Cases[i][0], 16)
        err = binary.Write(fp, binary.LittleEndian, uint32(len(a.Bytes())))
        if err != nil { panic(err) }
        fp.Write(rev(a.Bytes()))


        b, _ := new(big.Int).SetString(Cases[i][1], 16)
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
