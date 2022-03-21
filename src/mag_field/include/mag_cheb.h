#ifndef MAG_FIELD_MAG_CHEB_H
#define MAG_FIELD_MAG_CHEB_H

#include <functional>
#include <memory>
#include <string>

#include <glbinding/gl46ext/gl.h>
#include <glm/glm.hpp>

#include <glm_helper.h>

#ifdef USE_ROOT
#include "alice_field.h"
#endif

namespace mag_field {
	class mag_cheb {
	public:
		static constexpr int DIMENSIONS = 3;

		static constexpr int SOL_Z_SEGS = 29;
		static constexpr int SOL_P_SEGS = 278;
		static constexpr int SOL_R_SEGS = 3492;

		static constexpr int SOL_PARAMS = 1534;
		static constexpr int SOL_COLS = 9931;
		static constexpr int SOL_COEFFS_PER_COL = 48444;
		static constexpr int SOL_COEFFS = 110683;

		static constexpr int DIP_Z_SEGS = 89;
		static constexpr int DIP_Y_SEGS = 1295;
		static constexpr int DIP_X_SEGS = 12019;

		static constexpr int DIP_PARAMS = 1482;
		static constexpr int DIP_COLS = 16108;
		static constexpr int DIP_COEFFS_PER_COL = 56528;
		static constexpr int DIP_COEFFS = 171958;

		static constexpr float SOL_MIN_Z = -550.f;
		static constexpr float SOL_MAX_Z = 850.f;
		static constexpr float DIP_MIN_Z = -1760.f;
		static constexpr float DIP_MAX_Z = -532.46997f;

		static constexpr float MIN_Z = DIP_MIN_Z;
		static constexpr float MAX_Z = SOL_MAX_Z;

		using Sol_Segments = struct {
			glm::float32 SegZSol[SOL_Z_SEGS];

			glm::int32 BegSegPSol[SOL_Z_SEGS];
			glm::int32 NSegPSol[SOL_Z_SEGS];

			glm::float32 SegPSol[SOL_P_SEGS];

			glm::int32 BegSegRSol[SOL_P_SEGS];
			glm::int32 NSegRSol[SOL_P_SEGS];
			glm::float32 SegRSol[SOL_R_SEGS];

			glm::int32 SegIDSol[SOL_R_SEGS];
		};

		using Sol_Params = struct {
			glm::float32 BOffsets[DIMENSIONS * SOL_PARAMS];
			glm::float32 BScales[DIMENSIONS * SOL_PARAMS];
			glm::float32 BMin[DIMENSIONS * SOL_PARAMS];
			glm::float32 BMax[DIMENSIONS * SOL_PARAMS];

			glm::int32 NRows[DIMENSIONS * SOL_PARAMS];
			glm::int32 ColsForRowOffset[DIMENSIONS * SOL_PARAMS];
			glm::int32 CofsForRowOffset[DIMENSIONS * SOL_PARAMS];

			glm::int32 NColsPerRow[SOL_COLS];
			glm::int32 CofsPerColOffset[SOL_COLS];

			glm::int32 NCofsPerCol[SOL_COEFFS_PER_COL];
			glm::int32 PerColCoefOffset[SOL_COEFFS_PER_COL];

			glm::float32 Coeffs[SOL_COEFFS];
		};

		using Dip_Segments = struct {
			glm::float32 SegZDip[DIP_Z_SEGS];

			glm::int32 BegSegYDip[DIP_Z_SEGS];
			glm::int32 NSegYDip[DIP_Z_SEGS];

			glm::float32 SegYDip[DIP_Y_SEGS];

			glm::int32 BegSegXDip[DIP_Y_SEGS];
			glm::int32 NSegXDip[DIP_Y_SEGS];
			glm::float32 SegXDip[DIP_X_SEGS];

			glm::int32 SegIDDip[DIP_X_SEGS];
		};

		using Dip_Params = struct {
			glm::float32 BOffsets[DIMENSIONS * DIP_PARAMS];
			glm::float32 BScales[DIMENSIONS * DIP_PARAMS];
			glm::float32 BMin[DIMENSIONS * DIP_PARAMS];
			glm::float32 BMax[DIMENSIONS * DIP_PARAMS];

			glm::int32 NRows[DIMENSIONS * DIP_PARAMS];
			glm::int32 ColsForRowOffset[DIMENSIONS * DIP_PARAMS];
			glm::int32 CofsForRowOffset[DIMENSIONS * DIP_PARAMS];

			glm::int32 NColsPerRow[DIP_COLS];
			glm::int32 CofsPerColOffset[DIP_COLS];

			glm::int32 NCofsPerCol[DIP_COEFFS_PER_COL];
			glm::int32 PerColCoefOffset[DIP_COEFFS_PER_COL];

			glm::float32 Coeffs[DIP_COEFFS];
		};

	private:
		Sol_Segments sol_segment{};
		Sol_Params sol_params{};

		Dip_Segments dip_segment{};
		Dip_Params dip_params{};

		int solSegCache;
		int dipSegCache;

#ifdef USE_ROOT
		std::unique_ptr<alice_field> aliceField;
#endif

		static constexpr int MAX_CHEB_ORDER = 32;

		void loadSolSegmentLUT(const std::string &filename);

		void loadSolParamsLUT(const std::string &filename);

		void loadDipSegmentLUT(const std::string &filename);

		void loadDipParamsLUT(const std::string &filename);

		glm::float32 cheb1DArray(glm::float32 x, const glm::float32 *arr, int ncf) const noexcept;

		glm::float32 cheb1DParams(glm::float32 x, const glm::float32 *Coeffs, int coeff_offset, int ncf) const noexcept;

		int findSolSegment(const glm::vec3 &pos) const noexcept;

		int findDipSegment(const glm::vec3 &pos) const noexcept;

		glm::vec3 mapToInternalSol(int segID, const glm::vec3 &rphiz) const noexcept;

		glm::vec3 mapToInternalDip(int segID, const glm::vec3 &pos) const noexcept;

		bool IsBetween(const glm::vec3 &sMin, const glm::vec3 &val, const glm::vec3 &sMax) const noexcept;

		bool IsInsideSol(int segID, const glm::vec3 &rphiz) const noexcept;

		bool IsInsideDip(int segID, const glm::vec3 &pos) const noexcept;

		glm::float32 Eval3DSol(int segID, int dim, const glm::vec3 &internal) const noexcept;

		glm::vec3 EvalSol(int segID, const glm::vec3 &rphiz) const noexcept;

		glm::float32 Eval3DDip(int segID, int dim, const glm::vec3 &internal) const noexcept;

		glm::vec3 EvalDip(int segID, const glm::vec3 &pos) const noexcept;

		glm::vec3 CarttoCyl(const glm::vec3 &pos) const noexcept;

		glm::vec3 CyltoCartCylB(const glm::vec3 &rphiz, const glm::vec3 &brphiz) const noexcept;

		glm::vec3 SolDipField(const glm::vec3 &pos, const bool useCache) noexcept;

		glm::vec3 MachineField(const glm::vec3 &pos) const noexcept;

	public:
		mag_cheb();

		void resetCache() {
			solSegCache = -1;
			dipSegCache = -1;
		};

		glm::vec3 Field(const glm::vec3 &pos, bool useCache = false) noexcept;

		Sol_Segments *getSolSegmentsPtr() { return &sol_segment; };

		Dip_Segments *getDipSegmentsPtr() { return &dip_segment; };

		Sol_Params *getSolParamsPtr() { return &sol_params; };

		Dip_Params *getDipParamsPtr() { return &dip_params; };

		void appendDipPages(common::unordered_ivec3_set &pages, const std::function<glm::ivec3(const glm::ivec3 &)> &worldToTex, const glm::ivec3 &texSizeInPages, const glm::ivec3 &pageSizes);

		void appendSolPages(common::unordered_ivec3_set &pages, const std::function<glm::ivec3(const glm::ivec3 &)> &worldToTex, const glm::ivec3 &texSizeInPages, const glm::ivec3 &pageSizes);
	};
}// namespace mag_field


#endif//MAG_FIELD_MAG_CHEB_H
