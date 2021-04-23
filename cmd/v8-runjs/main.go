// v8-runjs is a command-line tool to run javascript.
//
// It's like node, but less useful.
//
// It runs the javascript files provided on the commandline in order until
// it finishes or an error occurs. If no files are provided, this will enter a
// REPL mode where you can interactively run javascript.
//
// Other than the standard javascript environment, it provides console.*:
//   console.log, console.info: write args to stdout
//   console.warn:              write args to stderr in yellow
//   console.error:             write args to stderr in scary red
//
// Sooo... you can run your JS and print to the screen.
package main

import (
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"

	v8 "github.com/jviksne/v8go"
	"github.com/jviksne/v8go/v8console"
	"github.com/peterh/liner"
)

const (
	kRESET = "\033[0m"
	kRED   = "\033[91m"
)

func main() {
	flag.Parse()
	v8.Init("")
	isol, err := v8.NewIsolate()
	if err != nil {
		log.Fatal(err)
	}
	ctx := isol.NewContext()
	v8console.Config{"", os.Stdout, os.Stderr, true}.Inject(ctx)

	for _, filename := range flag.Args() {
		data, err := ioutil.ReadFile(filename)
		failOnError(err)
		_, err = ctx.Eval(string(data), filename)
		failOnError(err)
	}

	if flag.NArg() == 0 {
		s := liner.NewLiner()
		s.SetMultiLineMode(true)
		defer s.Close()
		for {
			jscode, err := s.Prompt("> ")
			if err == io.EOF {
				break
			}
			failOnError(err)
			s.AppendHistory(jscode)
			result, err := ctx.Eval(jscode, "<input>")
			if err != nil {
				fmt.Println(kRED, err, kRESET)
			} else {
				fmt.Println(result)
			}
		}
	}
}

func failOnError(err error) {
	if err != nil {
		panic(err)
	}
}
