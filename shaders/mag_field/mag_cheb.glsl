#define DIMENSIONS 3

#define SOL_Z_SEGS 29
#define SOL_P_SEGS 278
#define SOL_R_SEGS 3492

#define SOL_PARAMS 1534
#define SOL_COLS 9931
#define SOL_COEFFS_PER_COL 48444
#define SOL_COEFFS 110683

#define DIP_Z_SEGS 89
#define DIP_Y_SEGS 1295
#define DIP_X_SEGS 12019

#define DIP_PARAMS 1482
#define DIP_COLS 16108
#define DIP_COEFFS_PER_COL 56528
#define DIP_COEFFS 171958

#define MAX_CHEB_ORDER 32

layout(std430, binding = 1) restrict readonly buffer sol_segment_ssbo {
    float SegZSol[SOL_Z_SEGS];

    int BegSegPSol[SOL_Z_SEGS];
    int NSegPSol[SOL_Z_SEGS];

    float SegPSol[SOL_P_SEGS];
    int BegSegRSol[SOL_P_SEGS];
    int NSegRSol[SOL_P_SEGS];

    float SegRSol[SOL_R_SEGS];
    int SegIDSol[SOL_R_SEGS];
} sol_segment;

layout(std430, binding = 2) restrict readonly buffer dip_segment_ssbo {
    float SegZDip[DIP_Z_SEGS];

    int BegSegYDip[DIP_Z_SEGS];
    int NSegYDip[DIP_Z_SEGS];

    float SegYDip[DIP_Y_SEGS];
    int BegSegXDip[DIP_Y_SEGS];
    int NSegXDip[DIP_Y_SEGS];

    float SegXDip[DIP_X_SEGS];
    int SegIDDip[DIP_X_SEGS];
} dip_segment;

layout(std430, binding = 3) restrict readonly buffer sol_params_ssbo {
    float BOffsets[DIMENSIONS*SOL_PARAMS];
    float BScales[DIMENSIONS*SOL_PARAMS];
    float BMin[DIMENSIONS*SOL_PARAMS];
    float BMax[DIMENSIONS*SOL_PARAMS];
    int NRows[DIMENSIONS*SOL_PARAMS];
    int ColsForRowOffset[DIMENSIONS*SOL_PARAMS];
    int CofsForRowOffset[DIMENSIONS*SOL_PARAMS];

    int NColsPerRow[SOL_COLS];
    int CofsPerColOffset[SOL_COLS];

    int NCofsPerCol[SOL_COEFFS_PER_COL];
    int PerColCoefOffset[SOL_COEFFS_PER_COL];

    float Coeffs[SOL_COEFFS];
} sol_params;

layout(std430, binding = 4) restrict readonly buffer dip_params_ssbo {
    float BOffsets[DIMENSIONS*DIP_PARAMS];
    float BScales[DIMENSIONS*DIP_PARAMS];
    float BMin[DIMENSIONS*DIP_PARAMS];
    float BMax[DIMENSIONS*DIP_PARAMS];
    int NRows[DIMENSIONS*DIP_PARAMS];
    int ColsForRowOffset[DIMENSIONS*DIP_PARAMS];
    int CofsForRowOffset[DIMENSIONS*DIP_PARAMS];

    int NColsPerRow[DIP_COLS];
    int CofsPerColOffset[DIP_COLS];

    int NCofsPerCol[DIP_COEFFS_PER_COL];
    int PerColCoefOffset[DIP_COEFFS_PER_COL];

    float Coeffs[DIP_COEFFS];
} dip_params;

//layout(std430, binding = 5) restrict writeonly buffer debug_ssbo {
//    int xid;
//    int xBeg;
//    int yid;
//    int yBeg;
//    int zid;
//    int segID;
//} debug;

float tmpCfs1[MAX_CHEB_ORDER];
float tmpCfs0[MAX_CHEB_ORDER];

vec3 CarttoCyl(vec3 pos) {
    return vec3(length(pos.xy), atan(pos.y, pos.x), pos.z);
}

int findSolSegment(vec3 pos) {
    int rid,pid,zid;

    //GPU cache line is 128 bytes (32 floats), linear search might not be too bad here
    for(zid=0; zid < SOL_Z_SEGS; zid++) if(pos.z<sol_segment.SegZSol[zid]) break;
    if(--zid < 0) zid = 0;

    int psegBeg = sol_segment.BegSegPSol[zid];
    for(pid=0;pid<sol_segment.NSegPSol[zid];pid++) if(pos.y<sol_segment.SegPSol[psegBeg+pid]) break;
    if(--pid < 0) pid = 0;
    pid += psegBeg;

    int rsegBeg = sol_segment.BegSegRSol[pid];
    for(rid=0;rid<sol_segment.NSegRSol[pid];rid++) if(pos.x<sol_segment.SegRSol[rsegBeg+rid]) break;
    if(--rid < 0) rid = 0;
    rid += rsegBeg;

    return sol_segment.SegIDSol[rid];
}

int findDipSegment(vec3 pos) {
    int xid,yid,zid;

    //GPU cache line is 128 bytes (32 floats), linear search might not be too bad here
    for(zid=0; zid < DIP_Z_SEGS; zid++) if(pos.z<dip_segment.SegZDip[zid]) break;
    if(--zid < 0) zid = 0;

    int ysegBeg = dip_segment.BegSegYDip[zid];
    for(yid=0;yid<dip_segment.NSegYDip[zid];yid++) if(pos.y<dip_segment.SegYDip[ysegBeg+yid]) break;
    if(--yid < 0) yid = 0;
    yid += ysegBeg;

    int xsegBeg = dip_segment.BegSegXDip[yid];
    for(xid=0;xid<dip_segment.NSegXDip[yid];xid++) if(pos.x<dip_segment.SegXDip[xsegBeg+xid]) break;
    if(--xid < 0) xid = 0;
    xid += xsegBeg;

    return dip_segment.SegIDDip[xid];
}

vec3 mapToInternalSol(int segID, vec3 rphiz) {
    int index = DIMENSIONS*segID;
    vec3 offsets = vec3(sol_params.BOffsets[index+0], sol_params.BOffsets[index+1], sol_params.BOffsets[index+2]);
    vec3 scales = vec3(sol_params.BScales[index+0], sol_params.BScales[index+1], sol_params.BScales[index+2]);

    return (rphiz-offsets)*scales;
}

vec3 mapToInternalDip(int segID, vec3 pos) {
    int index = DIMENSIONS*segID;
    vec3 offsets = vec3(dip_params.BOffsets[index+0], dip_params.BOffsets[index+1], dip_params.BOffsets[index+2]);
    vec3 scales = vec3(dip_params.BScales[index+0], dip_params.BScales[index+1], dip_params.BScales[index+2]);

    return (pos-offsets)*scales;
}

float cheb1DArray(float x, float arr[MAX_CHEB_ORDER], int ncf) {
    if(ncf <= 0)
        return 0.0f;

    float b0 = arr[--ncf], b1=0, b2=0, x2=x+x;
    --ncf;

    for (int i=ncf;i>=0;i--) {
        b2 = b1;
        b1 = b0;
        b0 = arr[i] + x2*b1 -b2;
    }
    return b0 - x*b1;
}

float cheb1DParamsSol(float x, int coeff_offset, int ncf) {
    if(ncf <= 0)
        return 0.0f;

    float b0 = sol_params.Coeffs[coeff_offset + (--ncf)], b1=0, b2=0, x2=x+x;
    --ncf;

    for (int i=ncf;i>=0;i--) {
        b2 = b1;
        b1 = b0;
        b0 = sol_params.Coeffs[coeff_offset + i] + x2*b1 -b2;
    }
    return b0 - x*b1;
}

float cheb1DParamsDip(float x, int coeff_offset, int ncf) {
    if(ncf <= 0)
        return 0.0f;

    float b0 = dip_params.Coeffs[coeff_offset + (--ncf)], b1=0, b2=0, x2=x+x;
    --ncf;

    for (int i=ncf;i>=0;i--) {
        b2 = b1;
        b1 = b0;
        b0 = dip_params.Coeffs[coeff_offset + i] + x2*b1 -b2;
    }
    return b0 - x*b1;
}

float cheb1DfastArray(float x, float arr[MAX_CHEB_ORDER], int ncf) {
    if(ncf <= 0)
        return 0.0f;

    float x2 = 2*x;

    vec3 b = vec3(arr[--ncf], 0, 0);
    --ncf;

    vec3 t1 = vec3(1, x2, -1);

    for (int i=ncf;i>=0;i--) {
        b.zy = b.yx;
        b.x = arr[i];
        b.x = dot(t1, b);
    }

    vec3 t = vec3(1, -x, 0);
    return dot(t, b);
}

float cheb1DfastParamsSol(float x, int coeff_offset, int ncf) {
    if(ncf <= 0)
    return 0.0f;

    float x2 = 2*x;

    vec3 b = vec3(sol_params.Coeffs[coeff_offset + (--ncf)], 0, 0);
    --ncf;

    vec3 t1 = vec3(1, x2, -1);

    for (int i=ncf;i>=0;i--) {
        b.zy = b.yx;
        b.x = sol_params.Coeffs[coeff_offset + i];
        b.x = dot(t1, b);
    }

    vec3 t = vec3(1, -x, 0);
    return dot(t, b);
}

float cheb1DfastParamsDip(float x, int coeff_offset, int ncf) {
    if(ncf <= 0)
    return 0.0f;

    float x2 = 2*x;

    vec3 b = vec3(dip_params.Coeffs[coeff_offset + (--ncf)], 0, 0);
    --ncf;

    vec3 t1 = vec3(1, x2, -1);

    for (int i=ncf;i>=0;i--) {
        b.zy = b.yx;
        b.x = dip_params.Coeffs[coeff_offset + i];
        b.x = dot(t1, b);
    }

    vec3 t = vec3(1, -x, 0);
    return dot(t, b);
}

bool IsBetween(vec3 sMin, vec3 val, vec3 sMax) {
    //return all(equal(val, clamp(sMin, val, sMax)));

    return all(lessThanEqual(sMin, val)) && all(lessThanEqual(val, sMax));
}

bool IsInsideSol(int segID, vec3 rphiz) {
    int index = DIMENSIONS*segID;

    vec3 seg_min = vec3(sol_params.BMin[index+0], sol_params.BMin[index+1], sol_params.BMin[index+2]);
    vec3 seg_max = vec3(sol_params.BMax[index+0], sol_params.BMax[index+1], sol_params.BMax[index+2]);

    return IsBetween(seg_min, rphiz, seg_max);
}

bool IsInsideDip(int segID, vec3 pos) {
    int index = DIMENSIONS*segID;

    vec3 seg_min = vec3(dip_params.BMin[index+0], dip_params.BMin[index+1], dip_params.BMin[index+2]);
    vec3 seg_max = vec3(dip_params.BMax[index+0], dip_params.BMax[index+1], dip_params.BMax[index+2]);

    return IsBetween(seg_min, pos, seg_max);
}

float Eval3DSol(int segID, int dim, vec3 internal) {
    int index = DIMENSIONS*segID;
    int n_rows = sol_params.NRows[index+dim];
    int cols_for_row_offset = sol_params.ColsForRowOffset[index+dim];
    int coeffs_for_row_offset = sol_params.CofsForRowOffset[index+dim];

    for(int row = 0; row < n_rows; row++) {
        int n_cols = sol_params.NColsPerRow[cols_for_row_offset+row];
        int coeff_per_col_offset = sol_params.CofsPerColOffset[cols_for_row_offset+row];

        for(int col = 0; col < n_cols; col++) {
            int n_coeffs = sol_params.NCofsPerCol[coeff_per_col_offset+col];
            int per_col_coeff_offset = sol_params.PerColCoefOffset[coeff_per_col_offset+col];

            int coeffs_offset = coeffs_for_row_offset + per_col_coeff_offset;

            tmpCfs1[col] = cheb1DfastParamsSol(internal.z, coeffs_offset,n_coeffs);
        }
        tmpCfs0[row] = cheb1DfastArray(internal.y, tmpCfs1, n_cols);
    }

    return cheb1DfastArray(internal.x, tmpCfs0, n_rows);
}

vec3 EvalSol(int segID, vec3 rphiz) {
    vec3 internal = mapToInternalSol(segID, rphiz);
    return vec3(Eval3DSol(segID, 0, internal), Eval3DSol(segID, 1, internal), Eval3DSol(segID, 2, internal));
}

float Eval3DDip(int segID, int dim, vec3 internal) {
    int index = DIMENSIONS*segID;
    int n_rows = dip_params.NRows[index+dim];
    int cols_for_row_offset = dip_params.ColsForRowOffset[index+dim];
    int coeffs_for_row_offset = dip_params.CofsForRowOffset[index+dim];

    for(int row = 0; row < n_rows; row++) {
        int n_cols = dip_params.NColsPerRow[cols_for_row_offset+row];
        int coeff_per_col_offset = dip_params.CofsPerColOffset[cols_for_row_offset+row];

        for(int col = 0; col < n_cols; col++) {
            int n_coeffs = dip_params.NCofsPerCol[coeff_per_col_offset+col];
            int per_col_coeff_offset = dip_params.PerColCoefOffset[coeff_per_col_offset+col];

            int coeffs_offset = coeffs_for_row_offset + per_col_coeff_offset;

            tmpCfs1[col] = cheb1DfastParamsDip(internal.z, coeffs_offset,n_coeffs);
        }
        tmpCfs0[row] = cheb1DfastArray(internal.y, tmpCfs1, n_cols);
    }

    return cheb1DfastArray(internal.x, tmpCfs0, n_rows);
}

vec3 EvalDip(int segID, vec3 pos) {
    vec3 internal = mapToInternalDip(segID, pos);
    return vec3(Eval3DDip(segID, 0, internal), Eval3DDip(segID, 1, internal), Eval3DDip(segID, 2, internal));
}
