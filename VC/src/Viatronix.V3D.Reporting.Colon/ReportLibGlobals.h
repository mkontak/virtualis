#ifndef __GLOBALS_H_
#define __GLOBALS_H_

const OLECHAR TITLE_TABLE_OF_REVISIONS[] = L"Revisions";
const OLECHAR TITLE_COLON_FINDINGS[] = L"ColonFindings";
const OLECHAR TITLE_OTHER_FINDINGS[] = L"OtherFindings";
const OLECHAR TITLE_NON_DISTENDED[] = L"NonDistendedFindings";

const BSTR HEADER_MAJOR_VERSION = L"MajorVersion";
const BSTR HEADER_MINOR_VERSION = L"MinorVersion";
const BSTR HEADER_COMPLETED = L"Completed";

const OLECHAR IMAGE_PROPERTY_BITMAP[] = L"Bitmap";
const OLECHAR IMAGE_PROPERTY_DEPTH_BUFFER[] = L"DepthBuffer";
const OLECHAR IMAGE_PROPERTY_POSITION[] = L"Position";
const OLECHAR IMAGE_PROPERTY_DIRECTION[] = L"Direction";
const OLECHAR IMAGE_PROPERTY_ORIENTATION[] = L"Orientation";
const OLECHAR IMAGE_PROPERTY_DISTANCE[] = L"Distance";

const BSTR ENTRY_PROPERTY_COLON_FINDING = L"ColonFinding";
const BSTR ENTRY_PROPERTY_COMMENTS = L"Comments";
const BSTR ENTRY_PROPERTY_NAME = L"Name";
const BSTR ENTRY_PROPERTY_LOCATION = L"Location";
const BSTR ENTRY_PROPERTY_MAXIMUM = L"Maximum";
const BSTR ENTRY_PROPERTY_VERSION = L"EntryVersion";
const BSTR ENTRY_PROPERTY_PERPENDICULAR = L"Perpendicular";
const BSTR ENTRY_PROPERTY_SHAPE = L"Shape";
const BSTR ENTRY_PROPERTY_SIZE_COMPARISON = L"SizeComparison";
const BSTR ENTRY_PROPERTY_TRANSLUCENCY = L"Translucency";
const BSTR ENTRY_PROPERTY_CAMERACOORDINATES = L"CameraCoordinates";
const BSTR ENTRY_PROPERTY_ORIGDISTANCETORECTUM = L"OrigDistanceToRectum";
const BSTR ENTRY_PROPERTY_OTHERFINDINGID = L"MapEntryToOtherFindingID";

const BSTR INFO_STORAGE_NAME = L"ReportInfo";
const BSTR INFO_PROPERTY_PATIENT_NAME = L"PatientName";
const BSTR INFO_PROPERTY_PATIENT_ID = L"PatientID";
const BSTR INFO_PROPERTY_PATIENT_BIRTHDAY = L"PatientBirthday";
const BSTR INFO_PROPERTY_PATIENT_AGE = L"PatientAge";
const BSTR INFO_PROPERTY_PATIENT_WEIGHT = L"PatientWeight";
const BSTR INFO_PROPERTY_PATIENT_SEX = L"PatientSex";
const BSTR INFO_PROPERTY_STUDY_DATE = L"StudyDate";
const BSTR INFO_PROPERTY_ENDOSCOPIST = L"Endoscopist";
const BSTR INFO_PROPERTY_REFERRING = L"Referring";
const BSTR INFO_PROPERTY_INSTITUTION_NAME = L"InsitituionName";
const BSTR INFO_PROPERTY_INDICATION = L"Indication";
const BSTR INFO_PROPERTY_INDICATION_OTHER = L"OtherIndication";
const BSTR INFO_PROPERTY_EXAM_QUALITY = L"ExamQuality";
const BSTR INFO_PROPERTY_RECOMMENDATION = L"Recommendation";
const BSTR INFO_PROPERTY_RECOMMENDATION_TYPE = L"RecommendationType";
const BSTR INFO_PROPERTY_RECOMMENDATION_UNIT = L"RecommendationUnit";
const BSTR INFO_PROPERTY_RECOMMENDATION_TIME =  L"RecommendationTime";
const BSTR INFO_PROPERTY_RECOMMENDATION_OTHER =  L"OtherRecommendation";
const BSTR INFO_PROPERTY_AUTHOR =  L"Author";
const BSTR INFO_PROPERTY_IMPRESSION =  L"Impression";
const BSTR INFO_PROPERTY_CRADS_CATEGORY = L"RecommendationCRADSCategory";
const BSTR INFO_PROPERTY_EXTRACOLONIC_CRADS_CATEGORY = L"InfoXCCRADSCat";

const BSTR FINDING_PROPERTY_FINDING= L"Finding";
const BSTR FINDING_PROPERTY_LOCATION= L"Location";
const BSTR FINDING_PROPERTY_MAX_SIZE = L"MaximumSize";
const BSTR FINDING_PROPERTY_PERP_SIZE = L"PerpendicularSize";
const BSTR FINDING_PROPERTY_LENGTH = L"Length";
const BSTR FINDING_PROPERTY_DEGREE = L"Degree";
const BSTR FINDING_PROPERTY_COMMENT = L"Comment";
const BSTR FINDING_PROPERTY_SIZE = L"Size";
const BSTR FINDING_PROPERTY_CRADS_CATEGORY = L"OtherCRADSCategory";

const BSTR NONDISTENDED_PROPERTY_QUATERNION = L"Quaternion";
const BSTR NONDISTENDED_PROPERTY_MATRIX = L"RotationMatrix";
const BSTR NONDISTENDED_PROPERTY_ZOOM = L"ZoomFactor";
const BSTR NONDISTENDED_PROPERTY_LOCATION = L"Location";
const BSTR NONDISTENDED_PROPERTY_TEXT = L"Text";
const BSTR NONDISTENDED_PROPERTY_IMAGE = L"Image";
const BSTR NONDISTENDED_PROPERTY_TRANSFERX = L"TransferX";
const BSTR NONDISTENDED_PROPERTY_TRANSFERY = L"TransferY";
const BSTR NONDISTENDED_PROPERTY_VIEWMODE = L"ViewMode";
const BSTR NONDISTENDED_PROPERTY_OFFSET = L"Offset";
const BSTR NONDISTENDED_PROPERTY_TYPE = L"Type";
const BSTR NONDISTENDED_PROPERTY_POSITION = L"Position";
const BSTR NONDISTENDED_PROPERTY_DIRECTION = L"Direction";
const BSTR NONDISTENDED_PROPERTY_START_POINT = L"StartPoint";
const BSTR NONDISTENDED_PROPERTY_END_POINT = L"EndPoint";


const DWORD_PTR MODE_STREAM_WRITER = STGM_READWRITE|STGM_SHARE_EXCLUSIVE;
const DWORD_PTR MODE_STREAM_READER = STGM_READ|STGM_SHARE_EXCLUSIVE;
const DWORD_PTR MODE_STREAM_CREATE = STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE;

const DWORD_PTR MODE_SUBSTORAGE_WRITER = STGM_TRANSACTED|STGM_READWRITE|STGM_SHARE_EXCLUSIVE;
const DWORD_PTR MODE_SUBSTORAGE_READER = STGM_TRANSACTED|STGM_READ|STGM_SHARE_EXCLUSIVE;
const DWORD_PTR MODE_SUBSTORAGE_CREATE = STGM_CREATE|STGM_TRANSACTED|STGM_READWRITE|STGM_SHARE_EXCLUSIVE;

const long CURRENT_MAJOR_VERSION = 1;
const long CURRENT_MINOR_VERSION = 0;

inline HRESULT WINAPI _This(void* pv, REFIID iid, void** ppvObject, DWORD_PTR) 
{
	ATLASSERT(iid == IID_NULL);
	*ppvObject = pv;
	return S_OK;
}

#define COM_INTERFACE_ENTRY_THIS() COM_INTERFACE_ENTRY_FUNC(IID_NULL, 0, _This)

template<class T>
inline T* GetCoClass(IUnknown* punk)
{
	T* p = NULL;
	
	punk->QueryInterface(IID_NULL, reinterpret_cast<void**>(&p));
	
	ATLASSERT(p);
	
	return p;
}

inline CComBSTR CreateIndentifier()
{
  GUID guid;
  CoCreateGuid(&guid);
  
  TCHAR szBuffer[250];
  wsprintf(
    szBuffer, 
    _T("%04X%02X%02X%02X%02X%02X%02X%02X%02X%02X"),
    guid.Data1, 
    guid.Data2, 
    guid.Data3,
    guid.Data4[0], 
    guid.Data4[1],
    guid.Data4[2], 
    guid.Data4[3], 
    guid.Data4[4], 
    guid.Data4[5], 
    guid.Data4[7]
    );
  
  return szBuffer;
}

inline 
HRESULT CreateStream(CComPtr<IStorage> spParentStg, const OLECHAR* pwcsName, CComPtr<IStream>& spStream)
{
   return spParentStg->CreateStream(pwcsName, MODE_STREAM_CREATE, 0, 0, &spStream);
}

inline 
HRESULT OpenStream(CComPtr<IStorage> spParentStg, const OLECHAR* pwcsName, CComPtr<IStream>& spStream, bool bReadOnly)
{
  return spParentStg->OpenStream(pwcsName, NULL, bReadOnly? MODE_STREAM_READER:MODE_STREAM_WRITER, 0, &spStream);
}

inline HRESULT CreateStorage(CComPtr<IStorage> spParentStg, const OLECHAR* pwcsName, CComPtr<IStorage>& spStg)
{
  return spParentStg->CreateStorage(pwcsName, MODE_SUBSTORAGE_CREATE, 0, 0, &spStg);
}

inline HRESULT OpenStorage(CComPtr<IStorage> spParentStg, const OLECHAR* pwcsName, CComPtr<IStorage>& spStg, bool bReadOnly)
{
  return spParentStg->OpenStorage(pwcsName, NULL, bReadOnly? MODE_SUBSTORAGE_READER : MODE_SUBSTORAGE_WRITER, NULL, 0, &spStg);
}

#endif