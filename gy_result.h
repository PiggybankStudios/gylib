/*
File:   gy_result.h
Author: Taylor Robbins
Date:   04\16\2024
Description:
	** Result_t is an enumeration that is used for many of our enumeration scenarios where:
	**  1. the full list of enum values is irrelevant (i.e. unapplicable values are not a problem)
	**  2. The integer values of enum entries are unimportant
	**  3. There are a small number of possibilities (or the possibilities are shared with many other enums)
	** With all this in mind, we can make a single enum that serves all these
	** disparate scenarios, so we don't have to create new enums all the time.
*/

#ifndef _GY_RESULT_H
#define _GY_RESULT_H

enum Result_t
{
	Result_None = 0,
	Result_Success,
	Result_Failure,
	Result_Unknown,
	Result_Finished,
	
	Result_FunctionMissing,
	Result_NotAFunction,
	Result_Exception,
	Result_CouldntOpenFile,
	Result_MissingFile,
	Result_EmptyFile,
	Result_MissingFilePrefix,
	Result_TokenBeforeFilePrefix,
	Result_InvalidFilePrefix,
	Result_MultipleFilePrefix,
	Result_MissingRequiredFields,
	Result_UnknownBoneName,
	Result_MissingOrCorruptBones,
	Result_MissingOrCorruptChildren,
	Result_UnknownParentBoneName,
	Result_ParseFailure,
	Result_UnsupportedApi,
	Result_ApiError,
	Result_TextureError,
	Result_OutTextureError,
	Result_AllocFailure,
	Result_UnknownExtension,
	Result_DecodeError,
	Result_NoSamples,
	Result_NoDecodedData,
	Result_InvalidChannelCount,
	Result_InvalidSampleRate,
	Result_NotEnoughMemory,
	Result_LinePieceCountIsWrong,
	Result_IntParseFailure,
	Result_FloatParseFailure,
	Result_IndexOutOfBounds,
	Result_FaceVertIndexCountIsWrong,
	Result_CouldntFindMaterialLibrary,
	Result_MaterialAttribBeforeNew,
	Result_UnknownMaterial,
	Result_FacesBeforeObjectStart,
	Result_FacesBeforeMaterialSelect,
	Result_CompilationNotSupported,
	Result_MissingDivider,
	Result_VertexCreationFailed,
	Result_VertexCompileFailed,
	Result_FragmentCreationFailed,
	Result_FragmentCompileFailed,
	Result_ShaderCreationFailed,
	Result_LinkingFailed,
	Result_VertexArrayCreationFailed,
	Result_MissingAttribute,
	Result_MissingUniform,
	Result_NoVertShaderPieces,
	Result_NoFragShaderPieces,
	Result_KeyValuePairBeforeFrameDefined,
	Result_XmlParsingError,
	Result_GroupTransformIsInvalid,
	Result_GroupOutsideSvg,
	Result_ShapeOutsideGroup,
	Result_MissingProperty,
	Result_StyleFormatError,
	Result_PathFormatError,
	Result_NoGroups,
	Result_SvgParsingError,
	Result_InvalidRIFF,
	Result_InvalidWAVE,
	Result_MissingFormatChunk,
	Result_MissingDataChunks,
	Result_UnsupportedFormat,
	Result_DataBeforeFormat,
	Result_InvalidDataChunkSize,
	Result_MissingModId,
	Result_MissingDisplayName,
	Result_MissingVersion,
	Result_VersionNotFirst,
	Result_MultipleVersions,
	Result_CouldntParseVersion,
	Result_InvalidVersion,
	Result_VersionIsTooNew,
	Result_EmptyName,
	Result_MultipleNames,
	Result_MissingName,
	Result_CouldntParseSeed,
	Result_MissingSeed,
	Result_MissingDifficulty,
	Result_CouldntParseDifficulty,
	Result_MissingQuote,
	Result_InvalidChar,
	Result_InvalidIdentifier,
	Result_InvalidOperator,
	Result_MissingLeftOperand,
	Result_InvalidLeftOperand,
	Result_MissingRightOperand,
	Result_InvalidRightOperand,
	Result_InvalidConstant,
	Result_MismatchParenthesis,
	Result_UnknownVariable,
	Result_UnknownFunction,
	Result_TooManyArguments,
	Result_EmptyArgument,
	Result_InvalidArgument,
	Result_MissingContext,
	Result_EmptyExpression,
	Result_InvalidStack,
	Result_StackOverflow,
	Result_InvalidSyntax,
	Result_MissingOperator,
	
	Result_NumValues,
};

const char* GetResultStr(Result_t enumValue)
{
	switch (enumValue)
	{
		case Result_None:                           return "None";
		case Result_Success:                        return "Success";
		case Result_Failure:                        return "Failure";
		case Result_Unknown:                        return "Unknown";
		case Result_Finished:                       return "Finished";
		case Result_FunctionMissing:                return "FunctionMissing";
		case Result_NotAFunction:                   return "NotAFunction";
		case Result_Exception:                      return "Exception";
		case Result_CouldntOpenFile:                return "CouldntOpenFile";
		case Result_MissingFile:                    return "MissingFile";
		case Result_EmptyFile:                      return "EmptyFile";
		case Result_MissingFilePrefix:              return "MissingFilePrefix";
		case Result_TokenBeforeFilePrefix:          return "TokenBeforeFilePrefix";
		case Result_InvalidFilePrefix:              return "InvalidFilePrefix";
		case Result_MultipleFilePrefix:             return "MultipleFilePrefix";
		case Result_MissingRequiredFields:          return "MissingRequiredFields";
		case Result_UnknownBoneName:                return "UnknownBoneName";
		case Result_MissingOrCorruptBones:          return "MissingOrCorruptBones";
		case Result_MissingOrCorruptChildren:       return "MissingOrCorruptChildren";
		case Result_UnknownParentBoneName:          return "UnknownParentBoneName";
		case Result_ParseFailure:                   return "ParseFailure";
		case Result_UnsupportedApi:                 return "UnsupportedApi";
		case Result_ApiError:                       return "ApiError";
		case Result_TextureError:                   return "TextureError";
		case Result_OutTextureError:                return "OutTextureError";
		case Result_AllocFailure:                   return "AllocFailure";
		case Result_UnknownExtension:               return "UnknownExtension";
		case Result_DecodeError:                    return "DecodeError";
		case Result_NoSamples:                      return "NoSamples";
		case Result_NoDecodedData:                  return "NoDecodedData";
		case Result_InvalidChannelCount:            return "InvalidChannelCount";
		case Result_InvalidSampleRate:              return "InvalidSampleRate";
		case Result_NotEnoughMemory:                return "NotEnoughMemory";
		case Result_LinePieceCountIsWrong:          return "LinePieceCountIsWrong";
		case Result_IntParseFailure:                return "IntParseFailure";
		case Result_FloatParseFailure:              return "FloatParseFailure";
		case Result_IndexOutOfBounds:               return "IndexOutOfBounds";
		case Result_FaceVertIndexCountIsWrong:      return "FaceVertIndexCountIsWrong";
		case Result_CouldntFindMaterialLibrary:     return "CouldntFindMaterialLibrary";
		case Result_MaterialAttribBeforeNew:        return "MaterialAttribBeforeNew";
		case Result_UnknownMaterial:                return "UnknownMaterial";
		case Result_FacesBeforeObjectStart:         return "FacesBeforeObjectStart";
		case Result_FacesBeforeMaterialSelect:      return "FacesBeforeMaterialSelect";
		case Result_CompilationNotSupported:        return "CompilationNotSupported";
		case Result_MissingDivider:                 return "MissingDivider";
		case Result_VertexCreationFailed:           return "VertexCreationFailed";
		case Result_VertexCompileFailed:            return "VertexCompileFailed";
		case Result_FragmentCreationFailed:         return "FragmentCreationFailed";
		case Result_FragmentCompileFailed:          return "FragmentCompileFailed";
		case Result_ShaderCreationFailed:           return "ShaderCreationFailed";
		case Result_LinkingFailed:                  return "LinkingFailed";
		case Result_VertexArrayCreationFailed:      return "VertexArrayCreationFailed";
		case Result_MissingAttribute:               return "MissingAttribute";
		case Result_MissingUniform:                 return "MissingUniform";
		case Result_NoVertShaderPieces:             return "NoVertShaderPieces";
		case Result_NoFragShaderPieces:             return "NoFragShaderPieces";
		case Result_KeyValuePairBeforeFrameDefined: return "KeyValuePairBeforeFrameDefined";
		case Result_XmlParsingError:                return "XmlParsingError";
		case Result_GroupTransformIsInvalid:        return "GroupTransformIsInvalid";
		case Result_GroupOutsideSvg:                return "GroupOutsideSvg";
		case Result_ShapeOutsideGroup:              return "ShapeOutsideGroup";
		case Result_MissingProperty:                return "MissingProperty";
		case Result_StyleFormatError:               return "StyleFormatError";
		case Result_PathFormatError:                return "PathFormatError";
		case Result_NoGroups:                       return "NoGroups";
		case Result_SvgParsingError:                return "SvgParsingError";
		case Result_InvalidRIFF:                    return "InvalidRIFF";
		case Result_InvalidWAVE:                    return "InvalidWAVE";
		case Result_MissingFormatChunk:             return "MissingFormatChunk";
		case Result_MissingDataChunks:              return "MissingDataChunks";
		case Result_UnsupportedFormat:              return "UnsupportedFormat";
		case Result_DataBeforeFormat:               return "DataBeforeFormat";
		case Result_InvalidDataChunkSize:           return "InvalidDataChunkSize";
		case Result_MissingModId:                   return "MissingModId";
		case Result_MissingDisplayName:             return "MissingDisplayName";
		case Result_MissingVersion:                 return "MissingVersion";
		case Result_VersionNotFirst:                return "VersionNotFirst";
		case Result_MultipleVersions:               return "MultipleVersions";
		case Result_CouldntParseVersion:            return "CouldntParseVersion";
		case Result_InvalidVersion:                 return "InvalidVersion";
		case Result_VersionIsTooNew:                return "VersionIsTooNew";
		case Result_EmptyName:                      return "EmptyName";
		case Result_MultipleNames:                  return "MultipleNames";
		case Result_MissingName:                    return "MissingName";
		case Result_CouldntParseSeed:               return "CouldntParseSeed";
		case Result_MissingSeed:                    return "MissingSeed";
		case Result_MissingDifficulty:              return "MissingDifficulty";
		case Result_CouldntParseDifficulty:         return "CouldntParseDifficulty";
		case Result_MissingQuote:                   return "MissingQuote";
		case Result_InvalidChar:                    return "InvalidChar";
		case Result_InvalidIdentifier:              return "InvalidChar";
		case Result_InvalidOperator:                return "InvalidOperator";
		case Result_MissingLeftOperand:             return "MissingLeftOperand";
		case Result_InvalidLeftOperand:             return "InvalidLeftOperand";
		case Result_MissingRightOperand:            return "MissingRightOperand";
		case Result_InvalidRightOperand:            return "InvalidRightOperand";
		case Result_InvalidConstant:                return "InvalidConstant";
		case Result_MismatchParenthesis:            return "MismatchParenthesis";
		case Result_UnknownVariable:                return "UnknownVariable";
		case Result_UnknownFunction:                return "UnknownFunction";
		case Result_TooManyArguments:               return "TooManyArguments";
		case Result_EmptyArgument:                  return "EmptyArgument";
		case Result_InvalidArgument:                return "InvalidArgument";
		case Result_MissingContext:                 return "MissingContext";
		case Result_EmptyExpression:                return "EmptyExpression";
		case Result_InvalidStack:                   return "InvalidStack";
		case Result_StackOverflow:                  return "StackOverflow";
		case Result_InvalidSyntax:                  return "InvalidSyntax";
		case Result_MissingOperator:                return "MissingOperator";
		default: return "Unknown";
	}
}

#endif //  _GY_RESULT_H
