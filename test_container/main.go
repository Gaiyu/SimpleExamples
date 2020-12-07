package main

import (
    "runtime"
    "time"
	"fmt"
)

func main() {
    quit := make(chan bool)
	fmt.Println("num cpu:", runtime.NumCPU())
    for i := 0; i != runtime.NumCPU(); i++ {
        go func() {
            for {
                select {
                case <-quit:
                    break
                default:
                }
            }
        }()
    }

    time.Sleep(time.Second * 60)
    for i := 0; i != runtime.NumCPU(); i++ {
        quit <- true
    }
}
