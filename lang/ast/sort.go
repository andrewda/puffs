// Copyright 2017 The Puffs Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package ast

import (
	t "github.com/google/puffs/lang/token"
)

func TopologicalSortStructs(ns []*Struct) (sorted []*Struct, ok bool) {
	// Algorithm is a depth-first search as per
	// https://en.wikipedia.org/wiki/Topological_sorting#Depth-first_search
	sorted = make([]*Struct, 0, len(ns))
	byName := map[t.ID]*Struct{}
	for _, n := range ns {
		byName[n.Name()] = n
	}
	marks := map[*Struct]uint8{}
	for _, n := range ns {
		if _, ok := marks[n]; !ok {
			sorted, ok = tssVisit(sorted, n, byName, marks)
			if !ok {
				return nil, false
			}
		}
	}
	return sorted, true
}

func tssVisit(dst []*Struct, n *Struct, byName map[t.ID]*Struct, marks map[*Struct]uint8) ([]*Struct, bool) {
	const (
		unmarked  = 0
		temporary = 1
		permanent = 2
	)
	switch marks[n] {
	case temporary:
		return nil, false
	case permanent:
		return dst, true
	}
	marks[n] = temporary

	for _, f := range n.Fields() {
		x := f.Field().XType().Innermost()
		if x.Decorator() != 0 {
			continue
		}
		if o := byName[x.Name()]; o != nil {
			var ok bool
			dst, ok = tssVisit(dst, o, byName, marks)
			if !ok {
				return nil, false
			}
		}
	}

	marks[n] = permanent
	return append(dst, n), true
}
