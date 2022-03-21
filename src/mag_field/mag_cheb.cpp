#include "mag_cheb.h"

#include <fstream>
#include <spdlog/spdlog.h>

#include <config.h>
#include <logger.h>
#include <measurements.h>

mag_field::mag_cheb::mag_cheb() {
#ifdef USE_ROOT
	aliceField = std::make_unique<alice_field>();
#else
	loadSolSegmentLUT("sol_segments.bin");
	loadSolParamsLUT("sol_params.bin");

	loadDipSegmentLUT("dip_segments.bin");
	loadDipParamsLUT("dip_params.bin");
#endif
}

void mag_field::mag_cheb::loadSolSegmentLUT(const std::string &filename) {
	std::ifstream file;

	file.open("data/" + filename, std::ifstream::binary);

	if (!file.good()) {
		spdlog::error("Cannot load segment file.");
	}

	std::int32_t NZSegSol, NPSegSol, NRSegSol;

	file.read(reinterpret_cast<char *>(&NZSegSol), sizeof(std::int32_t));
	assert(NZSegSol == mag_cheb::SOL_Z_SEGS);

	file.read(reinterpret_cast<char *>(this->sol_segment.SegZSol), NZSegSol * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->sol_segment.BegSegPSol), NZSegSol * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->sol_segment.NSegPSol), NZSegSol * sizeof(glm::int32));

	file.read(reinterpret_cast<char *>(&NPSegSol), sizeof(std::int32_t));
	assert(NPSegSol == mag_cheb::SOL_P_SEGS);

	file.read(reinterpret_cast<char *>(this->sol_segment.SegPSol), NPSegSol * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->sol_segment.BegSegRSol), NPSegSol * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->sol_segment.NSegRSol), NPSegSol * sizeof(glm::int32));

	file.read(reinterpret_cast<char *>(&NRSegSol), sizeof(std::int32_t));
	assert(NRSegSol == mag_cheb::SOL_R_SEGS);

	file.read(reinterpret_cast<char *>(this->sol_segment.SegRSol), NRSegSol * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->sol_segment.SegIDSol), NRSegSol * sizeof(glm::int32));

	file.close();
}

void mag_field::mag_cheb::loadDipSegmentLUT(const std::string &filename) {
	std::ifstream file;

	file.open("data/" + filename, std::ifstream::binary);

	if (!file.good()) {
		spdlog::error("Cannot load segment file.");
	}

	std::int32_t NZSegDip, NYSegDip, NXSegDip;

	file.read(reinterpret_cast<char *>(&NZSegDip), sizeof(std::int32_t));
	assert(NZSegDip == mag_cheb::DIP_Z_SEGS);

	file.read(reinterpret_cast<char *>(this->dip_segment.SegZDip), NZSegDip * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->dip_segment.BegSegYDip), NZSegDip * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->dip_segment.NSegYDip), NZSegDip * sizeof(glm::int32));

	file.read(reinterpret_cast<char *>(&NYSegDip), sizeof(std::int32_t));
	assert(NYSegDip == mag_cheb::DIP_Y_SEGS);

	file.read(reinterpret_cast<char *>(this->dip_segment.SegYDip), NYSegDip * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->dip_segment.BegSegXDip), NYSegDip * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->dip_segment.NSegXDip), NYSegDip * sizeof(glm::int32));

	file.read(reinterpret_cast<char *>(&NXSegDip), sizeof(std::int32_t));
	assert(NXSegDip == mag_cheb::DIP_X_SEGS);

	file.read(reinterpret_cast<char *>(this->dip_segment.SegXDip), NXSegDip * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->dip_segment.SegIDDip), NXSegDip * sizeof(glm::int32));

	file.close();
}

void mag_field::mag_cheb::loadSolParamsLUT(const std::string &filename) {
	std::ifstream file;

	file.open("data/" + filename, std::ifstream::binary);

	if (!file.good()) {
		spdlog::error("Cannot load params file.");
	}

	std::int32_t NParams;
	file.read(reinterpret_cast<char *>(&NParams), sizeof(std::int32_t));
	assert(NParams == mag_cheb::SOL_PARAMS);

	file.read(reinterpret_cast<char *>(this->sol_params.BOffsets), mag_cheb::DIMENSIONS * NParams * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->sol_params.BScales), mag_cheb::DIMENSIONS * NParams * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->sol_params.BMin), mag_cheb::DIMENSIONS * NParams * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->sol_params.BMax), mag_cheb::DIMENSIONS * NParams * sizeof(glm::float32));

	file.read(reinterpret_cast<char *>(this->sol_params.NRows), mag_cheb::DIMENSIONS * NParams * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->sol_params.ColsForRowOffset), mag_cheb::DIMENSIONS * NParams * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->sol_params.CofsForRowOffset), mag_cheb::DIMENSIONS * NParams * sizeof(glm::int32));

	std::int32_t NCols;
	file.read(reinterpret_cast<char *>(&NCols), sizeof(std::int32_t));
	assert(NCols == mag_cheb::SOL_COLS);

	file.read(reinterpret_cast<char *>(this->sol_params.NColsPerRow), NCols * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->sol_params.CofsPerColOffset), NCols * sizeof(glm::int32));

	std::int32_t NCoeffsPerCol;
	file.read(reinterpret_cast<char *>(&NCoeffsPerCol), sizeof(std::int32_t));
	assert(NCoeffsPerCol == mag_cheb::SOL_COEFFS_PER_COL);

	file.read(reinterpret_cast<char *>(this->sol_params.NCofsPerCol), NCoeffsPerCol * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->sol_params.PerColCoefOffset), NCoeffsPerCol * sizeof(glm::int32));

	std::int32_t NCoeffs;
	file.read(reinterpret_cast<char *>(&NCoeffs), sizeof(std::int32_t));
	assert(NCoeffs == mag_cheb::SOL_COEFFS);

	file.read(reinterpret_cast<char *>(this->sol_params.Coeffs), NCoeffs * sizeof(glm::float32));

	file.close();
}

void mag_field::mag_cheb::loadDipParamsLUT(const std::string &filename) {
	std::ifstream file;

	file.open("data/" + filename, std::ifstream::binary);

	if (!file.good()) {
		spdlog::error("Cannot load params file.");
	}

	std::int32_t NParams;
	file.read(reinterpret_cast<char *>(&NParams), sizeof(std::int32_t));
	assert(NParams == mag_cheb::DIP_PARAMS);

	file.read(reinterpret_cast<char *>(this->dip_params.BOffsets), mag_cheb::DIMENSIONS * NParams * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->dip_params.BScales), mag_cheb::DIMENSIONS * NParams * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->dip_params.BMin), mag_cheb::DIMENSIONS * NParams * sizeof(glm::float32));
	file.read(reinterpret_cast<char *>(this->dip_params.BMax), mag_cheb::DIMENSIONS * NParams * sizeof(glm::float32));

	file.read(reinterpret_cast<char *>(this->dip_params.NRows), mag_cheb::DIMENSIONS * NParams * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->dip_params.ColsForRowOffset), mag_cheb::DIMENSIONS * NParams * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->dip_params.CofsForRowOffset), mag_cheb::DIMENSIONS * NParams * sizeof(glm::int32));

	std::int32_t NCols;
	file.read(reinterpret_cast<char *>(&NCols), sizeof(std::int32_t));
	assert(NCols == mag_cheb::DIP_COLS);

	file.read(reinterpret_cast<char *>(this->dip_params.NColsPerRow), NCols * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->dip_params.CofsPerColOffset), NCols * sizeof(glm::int32));

	std::int32_t NCoeffsPerCol;
	file.read(reinterpret_cast<char *>(&NCoeffsPerCol), sizeof(std::int32_t));
	assert(NCoeffsPerCol == mag_cheb::DIP_COEFFS_PER_COL);

	file.read(reinterpret_cast<char *>(this->dip_params.NCofsPerCol), NCoeffsPerCol * sizeof(glm::int32));
	file.read(reinterpret_cast<char *>(this->dip_params.PerColCoefOffset), NCoeffsPerCol * sizeof(glm::int32));

	std::int32_t NCoeffs;
	file.read(reinterpret_cast<char *>(&NCoeffs), sizeof(std::int32_t));
	assert(NCoeffs == mag_cheb::DIP_COEFFS);

	file.read(reinterpret_cast<char *>(this->dip_params.Coeffs), NCoeffs * sizeof(glm::float32));

	file.close();
}

glm::vec3 mag_field::mag_cheb::Field(const glm::vec3 &pos, const bool useCache) noexcept {
#ifdef USE_ROOT
	return aliceField->Field(pos);
#else
	if (pos.z > mag_cheb::MIN_Z && pos.z < mag_cheb::MAX_Z) {
		return SolDipField(pos, useCache);
	}
	return MachineField(pos);
#endif
}

glm::vec3 mag_field::mag_cheb::MachineField(const glm::vec3 &pos) const noexcept {
	return glm::vec3(0);
}

glm::vec3 mag_field::mag_cheb::SolDipField(const glm::vec3 &pos, const bool useCache) noexcept {
	if (pos.z > mag_cheb::SOL_MIN_Z) {
		glm::vec3 rphiz = CarttoCyl(pos);

		if (useCache) {
			if (solSegCache >= 0 && IsInsideSol(solSegCache, rphiz)) {
				glm::vec3 brphiz = EvalSol(solSegCache, rphiz);
				return CyltoCartCylB(rphiz, brphiz);
			}
		}

		int segID = findSolSegment(rphiz);
		if (segID >= 0 && IsInsideSol(segID, rphiz)) {
			glm::vec3 brphiz = EvalSol(segID, rphiz);

			if (useCache) {
				solSegCache = segID;
			}

			return CyltoCartCylB(rphiz, brphiz);
		}
	}

	if (useCache) {
		if (dipSegCache >= 0 && IsInsideDip(dipSegCache, pos)) {
			return EvalDip(dipSegCache, pos);
		}
	}

	int segID = findDipSegment(pos);
	if (segID >= 0 && IsInsideDip(segID, pos)) {

		if (useCache) {
			dipSegCache = segID;
		}

		return EvalDip(segID, pos);
	}

	return glm::vec3(0);
}

glm::vec3 mag_field::mag_cheb::CarttoCyl(const glm::vec3 &pos) const noexcept {
	return glm::vec3(glm::length(glm::vec2(pos.x, pos.y)), glm::atan(pos.y, pos.x), pos.z);
}

glm::vec3 mag_field::mag_cheb::CyltoCartCylB(const glm::vec3 &rphiz, const glm::vec3 &brphiz) const noexcept {
	const float btr = glm::length(glm::vec2(brphiz.x, brphiz.y));
	const float psiPLUSphi = glm::atan(brphiz.y, brphiz.x) + rphiz.y;

	return glm::vec3(btr * glm::cos(psiPLUSphi), btr * glm::sin(psiPLUSphi), brphiz.z);
}

glm::vec3 mag_field::mag_cheb::EvalDip(int segID, const glm::vec3 &pos) const noexcept {
	const glm::vec3 internal = mapToInternalDip(segID, pos);
	return glm::vec3(Eval3DDip(segID, 0, internal), Eval3DDip(segID, 1, internal), Eval3DDip(segID, 2, internal));
}

float mag_field::mag_cheb::Eval3DDip(int segID, int dim, const glm::vec3 &internal) const noexcept {
	const int index = DIMENSIONS * segID;
	const int n_rows = dip_params.NRows[index + dim];
	const int cols_for_row_offset = dip_params.ColsForRowOffset[index + dim];
	const int coeffs_for_row_offset = dip_params.CofsForRowOffset[index + dim];

	glm::float32 tmpCfs1[MAX_CHEB_ORDER];
	glm::float32 tmpCfs0[MAX_CHEB_ORDER];

	for (int row = 0; row < n_rows; row++) {
		const int n_cols = dip_params.NColsPerRow[cols_for_row_offset + row];
		const int coeff_per_col_offset = dip_params.CofsPerColOffset[cols_for_row_offset + row];

		for (int col = 0; col < n_cols; col++) {
			const int n_coeffs = dip_params.NCofsPerCol[coeff_per_col_offset + col];
			const int per_col_coeff_offset = dip_params.PerColCoefOffset[coeff_per_col_offset + col];

			const int coeffs_offset = coeffs_for_row_offset + per_col_coeff_offset;

			tmpCfs1[col] = cheb1DParams(internal.z, dip_params.Coeffs, coeffs_offset, n_coeffs);
		}
		tmpCfs0[row] = cheb1DArray(internal.y, tmpCfs1, n_cols);
	}

	return cheb1DArray(internal.x, tmpCfs0, n_rows);
}

glm::vec3 mag_field::mag_cheb::EvalSol(int segID, const glm::vec3 &rphiz) const noexcept {
	const glm::vec3 internal = mapToInternalSol(segID, rphiz);
	return glm::vec3(Eval3DSol(segID, 0, internal), Eval3DSol(segID, 1, internal), Eval3DSol(segID, 2, internal));
}

glm::float32 mag_field::mag_cheb::Eval3DSol(int segID, int dim, const glm::vec3 &internal) const noexcept {
	const int index = DIMENSIONS * segID;
	const int n_rows = sol_params.NRows[index + dim];
	const int cols_for_row_offset = sol_params.ColsForRowOffset[index + dim];
	const int coeffs_for_row_offset = sol_params.CofsForRowOffset[index + dim];

	glm::float32 tmpCfs1[MAX_CHEB_ORDER];
	glm::float32 tmpCfs0[MAX_CHEB_ORDER];

	for (int row = 0; row < n_rows; row++) {
		const int n_cols = sol_params.NColsPerRow[cols_for_row_offset + row];
		const int coeff_per_col_offset = sol_params.CofsPerColOffset[cols_for_row_offset + row];

		for (int col = 0; col < n_cols; col++) {
			const int n_coeffs = sol_params.NCofsPerCol[coeff_per_col_offset + col];
			const int per_col_coeff_offset = sol_params.PerColCoefOffset[coeff_per_col_offset + col];

			const int coeffs_offset = coeffs_for_row_offset + per_col_coeff_offset;

			tmpCfs1[col] = cheb1DParams(internal.z, sol_params.Coeffs, coeffs_offset, n_coeffs);
		}
		tmpCfs0[row] = cheb1DArray(internal.y, tmpCfs1, n_cols);
	}

	return cheb1DArray(internal.x, tmpCfs0, n_rows);
}

bool mag_field::mag_cheb::IsInsideDip(int segID, const glm::vec3 &pos) const noexcept {
	const int index = DIMENSIONS * segID;

	const glm::vec3 seg_min = glm::vec3(dip_params.BMin[index + 0], dip_params.BMin[index + 1], dip_params.BMin[index + 2]);
	const glm::vec3 seg_max = glm::vec3(dip_params.BMax[index + 0], dip_params.BMax[index + 1], dip_params.BMax[index + 2]);

	return IsBetween(seg_min, pos, seg_max);
}

bool mag_field::mag_cheb::IsInsideSol(int segID, const glm::vec3 &rphiz) const noexcept {
	const int index = DIMENSIONS * segID;

	const glm::vec3 seg_min = glm::vec3(sol_params.BMin[index + 0], sol_params.BMin[index + 1], sol_params.BMin[index + 2]);
	const glm::vec3 seg_max = glm::vec3(sol_params.BMax[index + 0], sol_params.BMax[index + 1], sol_params.BMax[index + 2]);

	return IsBetween(seg_min, rphiz, seg_max);
}

bool mag_field::mag_cheb::IsBetween(const glm::vec3 &sMin, const glm::vec3 &val, const glm::vec3 &sMax) const noexcept {
	return glm::all(glm::lessThanEqual(sMin, val)) && glm::all(glm::lessThanEqual(val, sMax));
}

glm::float32 mag_field::mag_cheb::cheb1DParams(glm::float32 x, const glm::float32 *Coeffs, int coeff_offset, int ncf) const noexcept {
	if (ncf <= 0)
		return 0.0f;

	float b0 = Coeffs[coeff_offset + (--ncf)], b1 = 0, b2 = 0, x2 = x + x;
	--ncf;

	for (int i = ncf; i >= 0; i--) {
		b2 = b1;
		b1 = b0;
		b0 = Coeffs[coeff_offset + i] + x2 * b1 - b2;
	}
	return b0 - x * b1;
}

glm::float32 mag_field::mag_cheb::cheb1DArray(glm::float32 x, const glm::float32 *arr, int ncf) const noexcept {
	if (ncf <= 0)
		return 0.0f;

	const float x2 = 2 * x;

	glm::vec3 b = glm::vec3(arr[--ncf], 0, 0);
	--ncf;

	const glm::vec3 t1 = glm::vec3(1, x2, -1);

	for (int i = ncf; i >= 0; i--) {
		b.z = b.y;
		b.y = b.x;
		b.x = arr[i];
		b.x = dot(t1, b);
	}

	const glm::vec3 t = glm::vec3(1, -x, 0);
	return glm::dot(t, b);
}

glm::vec3 mag_field::mag_cheb::mapToInternalDip(int segID, const glm::vec3 &pos) const noexcept {
	const int index = DIMENSIONS * segID;
	const glm::vec3 offsets = glm::vec3(dip_params.BOffsets[index + 0], dip_params.BOffsets[index + 1], dip_params.BOffsets[index + 2]);
	const glm::vec3 scales = glm::vec3(dip_params.BScales[index + 0], dip_params.BScales[index + 1], dip_params.BScales[index + 2]);

	return (pos - offsets) * scales;
}

glm::vec3 mag_field::mag_cheb::mapToInternalSol(int segID, const glm::vec3 &rphiz) const noexcept {
	const int index = DIMENSIONS * segID;
	const glm::vec3 offsets = glm::vec3(sol_params.BOffsets[index + 0], sol_params.BOffsets[index + 1], sol_params.BOffsets[index + 2]);
	const glm::vec3 scales = glm::vec3(sol_params.BScales[index + 0], sol_params.BScales[index + 1], sol_params.BScales[index + 2]);

	return (rphiz - offsets) * scales;
}

int mag_field::mag_cheb::findDipSegment(const glm::vec3 &pos) const noexcept {
	int xid, yid, zid;

	for (zid = 0; zid < DIP_Z_SEGS; zid++)
		if (pos.z < dip_segment.SegZDip[zid]) break;
	if (--zid < 0) zid = 0;

	int ysegBeg = dip_segment.BegSegYDip[zid];
	for (yid = 0; yid < dip_segment.NSegYDip[zid]; yid++)
		if (pos.y < dip_segment.SegYDip[ysegBeg + yid]) break;
	if (--yid < 0) yid = 0;
	yid += ysegBeg;

	int xsegBeg = dip_segment.BegSegXDip[yid];
	for (xid = 0; xid < dip_segment.NSegXDip[yid]; xid++)
		if (pos.x < dip_segment.SegXDip[xsegBeg + xid]) break;
	if (--xid < 0) xid = 0;
	xid += xsegBeg;

	return dip_segment.SegIDDip[xid];
}

int mag_field::mag_cheb::findSolSegment(const glm::vec3 &rphiz) const noexcept {
	int rid, pid, zid;

	for (zid = 0; zid < SOL_Z_SEGS; zid++)
		if (rphiz.z < sol_segment.SegZSol[zid]) break;
	if (--zid < 0) zid = 0;

	int psegBeg = sol_segment.BegSegPSol[zid];
	for (pid = 0; pid < sol_segment.NSegPSol[zid]; pid++)
		if (rphiz.y < sol_segment.SegPSol[psegBeg + pid]) break;
	if (--pid < 0) pid = 0;
	pid += psegBeg;

	int rsegBeg = sol_segment.BegSegRSol[pid];
	for (rid = 0; rid < sol_segment.NSegRSol[pid]; rid++)
		if (rphiz.x < sol_segment.SegRSol[rsegBeg + rid]) break;
	if (--rid < 0) rid = 0;
	rid += rsegBeg;

	return sol_segment.SegIDSol[rid];
}

constexpr int int_floor(int x, int y) {
	return x / y;
}

constexpr int int_ceil(int x, int y) {
	return x / y + !!(x % y);
}

void mag_field::mag_cheb::appendDipPages(common::unordered_ivec3_set &pages, const std::function<glm::ivec3(const glm::ivec3 &)> &worldToTex, const glm::ivec3 &texSizeInPages, const glm::ivec3 &pageSizes) {
	common::field_logger()->info("Generating Dip pages...");

	const auto addToSet = [&](glm::ivec3 pos) {
		if (pos.s < 0)
			pos.s = 0;
		if (pos.t < 0)
			pos.t = 0;
		if (pos.p < 0)
			pos.p = 0;

		if (pos.s >= texSizeInPages.s)
			pos.s = texSizeInPages.s - 1;
		if (pos.t >= texSizeInPages.t)
			pos.t = texSizeInPages.t - 1;
		if (pos.p >= texSizeInPages.p)
			pos.p = texSizeInPages.p - 1;

		pages.emplace(pos * pageSizes);
	};

	for (std::size_t segID = 0; segID < DIP_PARAMS; ++segID) {
		const std::size_t index = DIMENSIONS * segID;

		glm::ivec3 v_min = worldToTex({dip_params.BMin[index + 0], dip_params.BMin[index + 1], dip_params.BMin[index + 2]});
		glm::ivec3 v_max = worldToTex({dip_params.BMax[index + 0], dip_params.BMax[index + 1], dip_params.BMax[index + 2]});

		v_min = v_min / pageSizes;
		v_max = v_max / pageSizes;

		for (std::size_t segX = v_min.x; segX <= v_max.x; ++segX) {
			for (std::size_t segY = v_min.y; segY <= v_max.y; ++segY) {
				for (std::size_t segZ = v_min.z; segZ <= v_max.z; ++segZ) {
					const glm::ivec3 pos{segX, segY, segZ};
					addToSet(pos);
				}
			}
		}
	}
}

void mag_field::mag_cheb::appendSolPages(common::unordered_ivec3_set &pages, const std::function<glm::ivec3(const glm::ivec3 &)> &worldToTex, const glm::ivec3 &texSizeInPages, const glm::ivec3 &pageSizes) {
	common::field_logger()->info("Generating Sol pages...");

	const auto alignPageBoundary = [&pageSizes](glm::ivec3 const &w) {
		return (w / pageSizes) * pageSizes;
	};

	const auto addToSet = [&](glm::ivec3 pos) {
		if (pos.s < 0)
			pos.s = 0;
		if (pos.t < 0)
			pos.t = 0;
		if (pos.p < 0)
			pos.p = 0;

		if (pos.s >= texSizeInPages.s)
			pos.s = texSizeInPages.s - 1;
		if (pos.t >= texSizeInPages.t)
			pos.t = texSizeInPages.t - 1;
		if (pos.p >= texSizeInPages.p)
			pos.p = texSizeInPages.p - 1;

		pages.emplace(pos * pageSizes);
	};

	const auto bresenhamFill = [&](std::size_t const &R, glm::ivec2 const &center, std::size_t const min_s, std::size_t const max_s) {
		const int xc = center.x, yc = center.y;

		const auto drawLine = [&](int xs, int xe, int y) {
			for (int x = xs - 1; x <= xe; ++x) {
				for (int z = min_s; z <= max_s; ++z) {
					const glm::ivec3 pos{z, x, y};
					addToSet(pos);
				}
			}
		};

		const auto drawMirrors = [&](int x, int y) {
			drawLine(xc - x, xc + x, yc + y);

			drawLine(xc - y, xc + y, yc + x);

			drawLine(xc - y, xc + y, yc - x);

			drawLine(xc - x, xc + x, yc - y);
		};

		int x = 0, y = R;
		int d = 3 - 2 * R;
		drawMirrors(x, y);
		while (y >= x) {
			x++;
			if (d > 0) {
				y--;
				d = d + 4 * (x - y) + 10;
			} else {
				d = d + 4 * x + 6;
			}
			drawMirrors(x, y);
		}
	};

	const auto solZ_min = alignPageBoundary(worldToTex(glm::ivec3(0, 0, mag_field::SolZMin)));
	const auto solZ_max = alignPageBoundary(worldToTex(glm::ivec3(0, 0, mag_field::SolZMax)));

	const auto center = glm::ivec2(texSizeInPages.t / 2, texSizeInPages.p / 2);
	const auto center_off = glm::ivec2(texSizeInPages.t / 2, texSizeInPages.p / 2 - 1);

	const auto min_s = solZ_min.s / pageSizes.s;
	const auto max_s = solZ_max.s / pageSizes.s;

	if constexpr (common::isDebug) {
		common::field_logger()->debug("Barrel S coords: [{},{}]", min_s, max_s);
	}

	const auto detectorDiameter = texSizeInPages.t;

	bresenhamFill(detectorDiameter / 2, center, min_s, max_s);
	bresenhamFill(detectorDiameter / 2, center_off, min_s, max_s);

	const auto pipeZ_min = alignPageBoundary(worldToTex(glm::ivec3(0, 0, mag_field::PipeZMin)));
	const auto pipeZ_max = alignPageBoundary(worldToTex(glm::ivec3(0, 0, mag_field::PipeZMax)));

	const auto pipe_min_s = pipeZ_min.s / pageSizes.s;
	const auto pipe_max_s = pipeZ_max.s / pageSizes.s;

	if constexpr (common::isDebug) {
		common::field_logger()->debug("LHC pipe S coords: [{},{}]", min_s, max_s);
	}

	const auto machineTubeDiameter = 200 / pageSizes.t / common::scale_tp;

	bresenhamFill(machineTubeDiameter / 2, center, pipe_min_s, pipe_max_s);
	bresenhamFill(machineTubeDiameter / 2, center_off, pipe_min_s, pipe_max_s);
}
