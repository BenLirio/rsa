package main

import (
    "math/big"
    "fmt"
    "encoding/binary"
    "os"
)

func main() {
    a := new(big.Int).SetUint64(uint64(1<<60))
    fmt.Printf("%.16x\n", a)
    fmt.Println(a.Bytes())
    bs := make([]byte, 8)
    binary.LittleEndian.PutUint64(bs, uint64(1<<60))
    fmt.Println(bs)

    fp, err := os.Create("number")
    if err != nil { panic(err) }
    binary.Write(fp, binary.BigEndian, uint64(1<<60))
    fp.Close()
}
