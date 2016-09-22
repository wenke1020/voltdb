/* This file is part of VoltDB.
 * Copyright (C) 2008-2016 VoltDB Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with VoltDB.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TupleOutputStream.h"
#include "tabletuple.h"
#include <limits>

namespace voltdb {

TupleOutputStream::TupleOutputStream(void *data, std::size_t length) :
    SerializeOutput<ReferenceSerializeOutput>(&m_serializer),
    m_rowCount(0),
    m_rowCountPosition(0),
    m_totalBytesSerialized(0)
{
    m_serializer.initializeWithPosition(data,length,0);
}

TupleOutputStream::~TupleOutputStream()
{
}

std::size_t TupleOutputStream::startRows(int32_t partitionId)
{
    writeInt(partitionId);
    m_rowCount = 0;
    m_rowCountPosition = reserveBytes(4);
    m_totalBytesSerialized = 0;
    return m_rowCountPosition;
}

std::size_t TupleOutputStream::writeRow(const TableTuple &tuple)
{
    const std::size_t startPos = position();
    tuple.serializeTo(*this, true);
    const std::size_t endPos = position();
    m_rowCount++;
    std::size_t bytesSerialized = endPos - startPos;
    m_totalBytesSerialized += bytesSerialized;
    return bytesSerialized;
}

bool TupleOutputStream::canFit(std::size_t nbytes) const
{
    return (m_serializer.remaining() >= nbytes + sizeof(int32_t));
}

void TupleOutputStream::endRows()
{
    writeIntAt(m_rowCountPosition, m_rowCount);
}

} // namespace voltdb
