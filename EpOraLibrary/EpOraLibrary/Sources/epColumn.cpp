#include "epColumn.h"
#include <assert.h>
#include <tchar.h>

#include "epOraError.h"
#include "epResultSet.h"
#include "epConnection.h"
using namespace epol;

Column::Column (ResultSet *rs, const char *name, unsigned int nameLen, unsigned short ociDataType, unsigned int maxDataSize, int fetchSize)
{
	// prerequisites
	assert (rs && name);

	initialize ();

	m_colName = std::string (name, nameLen);
	m_ociType = ociDataType;

	// decide the format we want oci to return data in (m_ociType member)
	switch (ociDataType)
	{
	case	SQLT_INT:	// integer
	case	SQLT_LNG:	// long
	case	SQLT_UIN:	// unsigned int

	case	SQLT_NUM:	// numeric
	case	SQLT_FLT:	// float
	case	SQLT_VNU:	// numeric with length
	case	SQLT_PDN:	// packed decimal
		m_ociType = SQLT_VNU;
		m_colName = DT_NUMBER;
		m_size = sizeof (OCINumber);
		break;

	case	SQLT_DAT:	// date
	case	SQLT_ODT:	// oci date - should not appear?
		m_ociType = SQLT_ODT;
		m_colName = DT_DATE;
		m_size = sizeof (OCIDate);
		break;

	case	SQLT_CHR:	// character string
	case	SQLT_STR:	// zero-terminated string
	case	SQLT_VCS:	// variable-character string
	case	SQLT_AFC:	// ansi fixed char
	case	SQLT_AVC:	// ansi var char
	case	SQLT_VST:	// oci string type
		m_ociType = SQLT_STR;
		m_colName = DT_TEXT;
		m_size = (maxDataSize + 1) * sizeof(TCHAR); // + 1 for terminating zero!
		break;

	default:
		throw (OraError(EC_UNSUP_ORA_TYPE, __FILE__, __LINE__, name));
	}

	// allocate memory for m_indicators, column lengths and fetched data
	m_indicators = new short [fetchSize];

	if (m_colType == DT_TEXT)
		// only text columns requires length
		m_dataLengths = new unsigned short [fetchSize];
	else
		m_dataLengths = NULL;

	m_fetchBuffer = new char [m_size * fetchSize];

	m_defineHandle = NULL;

	if (!m_indicators || !m_fetchBuffer)
	{
		cleanUp (); // because there could be some memory allocated
		// no memory
		throw (OraError(EC_NO_MEMORY, __FILE__, __LINE__));
	}

	m_resultSet = rs;
}


Column::~Column ()
{
	cleanUp ();
}


void Column::initialize ()
{
	m_colType = DT_UNKNOWN;
	m_ociType = 0;
	m_size = 0;
	m_indicators = NULL;
	m_dataLengths = NULL;
	m_fetchBuffer = NULL;
	m_defineHandle = NULL;
	m_resultSet = NULL;
}


void Column::cleanUp ()
{
	// set all to null to be save to call cleanup more than once for a single instance
	if (m_indicators) 
	{
		delete [] m_indicators; 
	}
	m_indicators = NULL;
	if (m_dataLengths) 
		delete [] m_dataLengths;
	m_dataLengths = NULL;
	if (m_fetchBuffer) 
		delete [] m_fetchBuffer;
	m_fetchBuffer = NULL;
}


bool Column::IsNull () const
{
	// prerequisites
	assert (m_resultSet);

	unsigned short	rowNo = static_cast <unsigned short> (m_resultSet->m_currentRow % m_resultSet->m_fetchCount);
	return (m_indicators [rowNo] == -1);
}


Pstr Column::ToString () const
{
	// prerequisites
	assert (m_resultSet);

	unsigned short	row_no = static_cast <unsigned short> (m_resultSet->m_currentRow % m_resultSet->m_fetchCount);
	if (m_colType == DT_TEXT &&	m_indicators [row_no] != -1)
		return (reinterpret_cast <Pstr> (m_fetchBuffer + m_size * row_no));
	else
		throw (OraError(EC_BAD_OUTPUT_TYPE, __FILE__, __LINE__));
}


double Column::ToDouble () const
{
	// prerequisites
	assert (m_resultSet);

	unsigned short	rowNo = static_cast <unsigned short> (m_resultSet->m_currentRow % m_resultSet->m_fetchCount);
	if (m_colType == DT_NUMBER &&
		m_indicators [rowNo] != -1)
	{
		double	value;
		int result = ::OCINumberToReal (m_resultSet->m_conn->m_errorHandle,reinterpret_cast <OCINumber *> (m_fetchBuffer) + rowNo, sizeof (double), &value);
		if (result == OCI_SUCCESS)
			return (value);
		else
			throw (OraError(result, m_resultSet->m_conn->m_errorHandle, __FILE__, __LINE__));
	}
	else
		throw (OraError(EC_BAD_OUTPUT_TYPE, __FILE__, __LINE__));
}


long
	Column::ToLong () const
{
	// prerequisites
	assert (m_resultSet);

	unsigned short	rowNo = static_cast <unsigned short> (m_resultSet->m_currentRow % m_resultSet->m_fetchCount);
	if (m_colType == DT_NUMBER &&
		m_indicators [rowNo] != -1)
	{
		long value;
		sword result = OCINumberToInt (m_resultSet->m_conn->m_errorHandle, reinterpret_cast <OCINumber *> (m_fetchBuffer) + rowNo, sizeof (long), OCI_NUMBER_SIGNED, &value);
		if (result == OCI_SUCCESS)
			return (value);
		else
			throw (OraError(result, m_resultSet->m_conn->m_errorHandle, __FILE__, __LINE__));
	}
	else
		throw (OraError(EC_BAD_OUTPUT_TYPE, __FILE__, __LINE__));
}


DateTime Column::ToDateTime () const
{
	// prerequisites
	assert (m_resultSet);

	unsigned short	rowNo = static_cast <unsigned short> (m_resultSet->m_currentRow % m_resultSet->m_fetchCount);
	if (m_colType == DT_DATE &&
		m_indicators [rowNo] != -1)
		return (DateTime (*(reinterpret_cast <OCIDate *> (m_fetchBuffer) + rowNo)));
	else
		throw (OraError(EC_BAD_OUTPUT_TYPE, __FILE__, __LINE__));
}
