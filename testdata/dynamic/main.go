package main

import (
    "math/big"
    "encoding/binary"
    "os"
)

type BigIntStr [2]string
type TestCase struct {
    ALen uint32
    Adata []byte
}

var Cases []BigIntStr = []BigIntStr{
    BigIntStr{"ffffffff", "ffffffff"},
    BigIntStr{"ffffffffffffffff", "ffffffffffffffff"},
    BigIntStr{"ffffffffffffffff", "1"},
    BigIntStr{"1", "244"},
}

func rev(b []byte) []byte {
    for i := 0; i < len(b)/2; i++ {
        b[i], b[len(b)-i-1] = b[len(b)-i-1], b[i]
    }
    return b
}

func WriteTestCase(fp *os.File, a, b *big.Int) {
        err := binary.Write(fp, binary.LittleEndian, uint32(len(a.Bytes())))
        if err != nil { panic(err) }
        fp.Write(rev(a.Bytes()))


        err = binary.Write(fp, binary.LittleEndian, uint32(len(b.Bytes())))
        if err != nil { panic(err) }
        fp.Write(rev(b.Bytes()))

        c := new(big.Int).Mul(a, b)
        err = binary.Write(fp, binary.LittleEndian, uint32(len(c.Bytes())))
        if err != nil { panic(err) }
        fp.Write(rev(c.Bytes()))

}


func main() {
    fp, err := os.Create("numbers")
    if err != nil { panic(err) }
    for i := 0; i < len(Cases); i++ {
        a, ok := new(big.Int).SetString(Cases[i][0], 16)
        if ok != true { panic("SetString Failed") }
        b, ok := new(big.Int).SetString(Cases[i][1], 16)
        if ok != true { panic("SetString Failed") }
        WriteTestCase(fp, a, b)
    }
    fp.Close()
}
