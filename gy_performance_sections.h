/*
File:   gy_performance_sections.h
Author: Taylor Robbins
Date:   04\09\2024
*/

#ifndef _GY_PERFORMANCE_SECTIONS_H
#define _GY_PERFORMANCE_SECTIONS_H

struct PerfSection_t
{
	const char* name;
	PerfTime_t time;
};

struct PerfSectionBundle_t
{
	MemArena_t* allocArena;
	VarArray_t sections; //PerfSection_t
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define PerfSection(sectionName) do                          \
{                                                            \
	if (perfSectionsEnabled)                                 \
	{                                                        \
		perfSections[perfSectionIndex].time = GetPerfTime(); \
		perfSections[perfSectionIndex].name = sectionName;   \
		perfSectionIndex++;                                  \
	}                                                        \
} while(0)
#define StartPerfSections(maxNumSections, firstSectionName, enabled) PerfSection_t perfSections[(maxNumSections)+1]; u64 perfSectionIndex = 0; bool perfSectionsEnabled = (enabled); PerfSection(firstSectionName)
#define EndPerfSections() PerfSection(nullptr)
#define ArePerfSectionsEnabled() (perfSectionsEnabled)
#define GetNumPerfSections() (perfSectionsEnabled ? (perfSectionIndex-1) : 0)

#define GetPerfSectionName(sectionIndex) GetPerfSectionName_(&perfSections[0], perfSectionIndex, ArrayCount(perfSections), (sectionIndex))
#define GetPerfSectionTimeByIndex(sectionIndex) GetPerfSectionTimeByIndex_(&perfSections[0], perfSectionIndex, ArrayCount(perfSections), (sectionIndex))
#define GetPerfSectionTime(sectionName) GetPerfSectionTime_(&perfSections[0], perfSectionIndex, ArrayCount(perfSections), (sectionName))
#define GetTotalPerfSectionsTime() GetTotalPerfSectionsTime_(&perfSections[0], perfSectionIndex, ArrayCount(perfSections))
#define BundlePerfSections(memArena, bundleOut) BundlePerfSections_(&perfSections[0], perfSectionIndex, ArrayCount(perfSections), (memArena), (bundleOut))

#define PrintPerfSections(printFunc, linePrefix) do                                                                              \
{                                                                                                                                \
	if (ArePerfSectionsEnabled())                                                                                                \
	{                                                                                                                            \
		for (u64 sectionIndex = 0; sectionIndex < GetNumPerfSections(); sectionIndex++)                                          \
		{                                                                                                                        \
			printFunc("%s%s: %.1lfms", (linePrefix), GetPerfSectionName(sectionIndex), GetPerfSectionTimeByIndex(sectionIndex)); \
		}                                                                                                                        \
		printFunc("%sTotal: %.1lfms", (linePrefix), GetTotalPerfSectionsTime());                                                 \
	}                                                                                                                            \
} while(0)

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	const char* GetPerfSectionName_(const PerfSection_t* sections, u64 numSections, u64 maxNumSections, u64 sectionIndex);
	r64 GetPerfSectionTimeByIndex_(const PerfSection_t* sections, u64 numSections, u64 maxNumSections, u64 sectionIndex);
	r64 GetPerfSectionTime_(const PerfSection_t* sections, u64 numSections, u64 maxNumSections, const char* sectionName);
	r64 GetTotalPerfSectionsTime_(const PerfSection_t* sections, u64 numSections, u64 maxNumSections);
	void BundlePerfSections_(const PerfSection_t* sections, u64 numSections, u64 maxNumSections, MemArena_t* memArena, PerfSectionBundle_t* bundleOut);
#else

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
const char* GetPerfSectionName_(const PerfSection_t* sections, u64 numSections, u64 maxNumSections, u64 sectionIndex)
{
	NotNull_(sections);
	Assert_(sectionIndex+1 < numSections);
	Assert_(numSections <= maxNumSections);
	return sections[sectionIndex].name;
}
r64 GetPerfSectionTimeByIndex_(const PerfSection_t* sections, u64 numSections, u64 maxNumSections, u64 sectionIndex)
{
	NotNull_(sections);
	Assert_(sectionIndex+1 < numSections);
	Assert_(numSections <= maxNumSections);
	const PerfSection_t* section = &sections[sectionIndex];
	const PerfSection_t* nextSection = &sections[sectionIndex+1];
	return GetPerfTimeDiff(&section->time, &nextSection->time);
}
r64 GetPerfSectionTime_(const PerfSection_t* sections, u64 numSections, u64 maxNumSections, const char* sectionName)
{
	bool foundSection = false;
	NotNull_(sections);
	Assert_(numSections <= maxNumSections);
	NotNull_(sectionName);
	for (u64 sIndex = 0; sIndex < numSections-1; sIndex++)
	{
		if (sections[sIndex].name != nullptr && MyStrCompareNt(sections[sIndex].name, sectionName) == 0)
		{
			foundSection = true;
			return GetPerfSectionTimeByIndex_(sections, numSections, maxNumSections, sIndex);
		}
	}
	Assert_(foundSection);
	return 0.0f;
}
r64 GetTotalPerfSectionsTime_(const PerfSection_t* sections, u64 numSections, u64 maxNumSections)
{
	NotNull_(sections);
	Assert_(numSections <= maxNumSections);
	if (numSections == 0) { return 0; }
	r64 result = 0.0;
	for (u64 sIndex = 0; sIndex < numSections-1; sIndex++)
	{
		result += GetPerfTimeDiff(&sections[sIndex].time, &sections[sIndex+1].time);
	}
	return result;
}

void BundlePerfSections_(const PerfSection_t* sections, u64 numSections, u64 maxNumSections, MemArena_t* memArena, PerfSectionBundle_t* bundleOut)
{
	NotNull_(sections);
	NotNull_(memArena);
	NotNull_(bundleOut);
	UNUSED(maxNumSections);
	
	ClearPointer(bundleOut);
	bundleOut->allocArena = memArena;
	CreateVarArray(&bundleOut->sections, memArena, sizeof(PerfSection_t), numSections);
	for (u64 sIndex = 0; sIndex < numSections; sIndex++)
	{
		PerfSection_t* newSection = VarArrayAdd(&bundleOut->sections, PerfSection_t);
		NotNull_(newSection);
		ClearPointer(newSection);
		if (sections[sIndex].name != nullptr)
		{
			newSection->name = AllocCharsAndFillNt(memArena, sections[sIndex].name);
			NotNull_(newSection->name);
		}
		newSection->time = sections[sIndex].time;
	}
}

#endif // GYLIB_HEADER_ONLY

#endif //  _GY_PERFORMANCE_SECTIONS_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Types
PerfSection_t
PerfSectionBundle_t
@Functions
#define PerfSection(sectionName)
#define StartPerfSections(maxNumSections, firstSectionName, enabled)
#define EndPerfSections()
#define ArePerfSectionsEnabled()
#define GetNumPerfSections()
#define GetPerfSectionName(sectionIndex)
#define GetPerfSectionTimeByIndex(sectionIndex)
#define GetPerfSectionTime(sectionName)
#define GetTotalPerfSectionsTime()
#define BundlePerfSections(memArena, bundleOut)
#define PrintPerfSections(printFunc, linePrefix)
*/
