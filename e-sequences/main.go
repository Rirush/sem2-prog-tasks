package main

import (
	"github.com/rs/zerolog"
	"github.com/rs/zerolog/log"
	"io/ioutil"
	"os"
	"regexp"
	"time"
)

type Entry struct {
	Start int
	Size int
}

func main() {
	t := time.Now()
	log.Logger = log.Output(zerolog.ConsoleWriter{Out: os.Stderr})
	zerolog.SetGlobalLevel(zerolog.InfoLevel)

	log.Info().Msg("reading file e2mil.txt")

	f, err := os.Open("e2mil.txt")
	if err != nil {
		log.Fatal().Err(err).Msg("failed to open file e2mil.txt")
	}
	data, err := ioutil.ReadAll(f)
	if err != nil {
		log.Fatal().Err(err).Msg("failed to read data from e2mil.txt")
	}
	log.Trace().Msg("file was read successfully")

	// much faster than manual filtering
	reg := regexp.MustCompile("[^0-9]+")
	eValue := string(data)
	eValue = reg.ReplaceAllString(eValue, "")
	log.Info().Int("data_size", len(eValue)).Msg("file was loaded")

	// initial pass to get all entry points
	entries := make(map[string][]int)
	for i, v := range eValue {
		entries[string(v)] = append(entries[string(v)], i)
	}

	current := make(map[string][]Entry)
	for key, entry := range entries {
		for _, v := range entry {
			if v == len(eValue)-1 {
				continue
			}
			current[key+string(eValue[v+1])] = append(current[key+string(eValue[v+1])], Entry{Start: v, Size: 2})
		}
	}
	for k, v := range current {
		if len(v) == 1 {
			delete(current, k)
		}
	}

	for len(current) != 1 {
		newCurrent := make(map[string][]Entry)
		for key, entries := range current {
			for _, entry := range entries {
				if entry.Start == len(eValue)-entry.Size {
					continue
				}
				newCurrent[key+string(eValue[entry.Start+entry.Size])] = append(newCurrent[key+string(eValue[entry.Start+entry.Size])], Entry{Start: entry.Start, Size: entry.Size+1})
			}
		}
		log.Debug().Int("new_size", len(newCurrent)).Msg("preprocessing done")
		for k, v := range newCurrent {
			if len(v) == 1 {
				delete(newCurrent, k)
			}
		}
		log.Debug().Int("new_size", len(newCurrent)).Msg("filtering	 done")
		if len(newCurrent) == 0 {
			break
		}
		current = newCurrent
		log.Debug().Int("size", len(current)).Msg("processing complete")
	}
	for k, v := range current {
		log.Info().Str("sequence", k).Interface("entries", v).Msg("repeat")
	}
	log.Info().Dur("took", time.Since(t)).Msg("done")
}
