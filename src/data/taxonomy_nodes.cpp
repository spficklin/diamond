/****
DIAMOND protein aligner
Copyright (C) 2013-2018 Benjamin Buchfink <buchfink@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
****/

#include <set>
#include "taxonomy_nodes.h"
#include "taxonomy.h"
#include "../util/log_stream.h"

using std::set;

void TaxonomyNodes::build(Serializer &out)
{
	task_timer timer("Building taxonomy nodes");
	out.unset(Serializer::VARINT);
	out << taxonomy.parent_;
	timer.finish();
	message_stream << taxonomy.parent_.size() << " taxonomy nodes processed." << endl;
}

TaxonomyNodes::TaxonomyNodes(Deserializer &in)
{
	in.varint = false;
	in >> parent_;
}

unsigned TaxonomyNodes::get_lca(unsigned t1, unsigned t2) const
{
	static const int max = 64;
	if (t1 == t2 || t2 == 0)
		return t1;
	if (t1 == 0)
		return t2;
	unsigned p = t2;
	set<unsigned> l;
	l.insert(p);
	int n = 0;
	do {
		p = get_parent(p);
		if (p == 0)
			return t1;
		l.insert(p);
		if (++n > max)
			throw std::runtime_error("Path in taxonomy too long (1).");
	} while (p != t1 && p != 1);
	if (p == t1)
		return p;
	p = t1;
	n = 0;
	while (l.find(p) == l.end()) {
		p = get_parent(p);
		if (p == 0)
			return t2;
		if (++n > max)
			throw std::runtime_error("Path in taxonomy too long (2).");
	}
	return p;
}