// Copyright 2019 bianchui. All rights reserved.
#ifndef xxprofile_version_h
#define xxprofile_version_h

#define XXPROFILE_VERSION_0 0
#define XXPROFILE_VERSION_1 1
#define XXPROFILE_VERSION_2 2

#define XXPROFILE_VERSION XXPROFILE_VERSION_2

// file format
// struct File { // start from here
//     struct FileHeader { // offset 0x00
//         /*0x00*/ uint32_t magic   = "XPAR";
//         /*0x04*/ uint32_t version = XXPROFILE_VERSION;
//         /*0x08*/ uint32_t flags   = Archive::Flags;
//         /*0x0C*/ uint32_t dummy   = 0;
//     };
//
//     struct Name {
//         if (XX_PROFILE_DEBUG_Name_Serialize) {
//             /*0x00*/ uint32_t id;
//         }
//         /*0x00+*/ uint32_t length;
//         /*0x04+*/ char name[length];
//     };
//
//     struct Names {
//         /*0x00*/ uint32_t nameCount;
//         if (nameCount > 0) {
//             /*0x04*/ uint32_t startIndex;
//             /*0x08*/ Name names[nameCount];
//         }
//     };
//
//     struct ChunkData_v2 {
//         /*0x00*/ uint32_t sizeOrg; // chunkNodeCount = sizeOrg / sizeof(XXProfileTreeNode)
//         /*0x04*/ uint32_t sizeCom;
//         if (sizeCom == 0) {
//             /*0x08*/ XXProfileTreeNode nodes[chunkNodeCount];
//         } else {
//             /*0x08*/ uint8_t compressed[sizeCom]; // expend to XXProfileTreeNode nodes[chunkNodeCount];
//         }
//     };
//
//     struct FrameData {
//         /*0x00*/ uint32_t frameId;
//         /*0x04*/ Names names;
//         /*0x0?*/ uint32_t nodeCount;
//         if (fileHdr.version == XXPROFILE_VERSION_1) {
//             /*    */ XXProfileTreeNode nodes[nodeCount];
//         } else if (fileHdr.version >= XXPROFILE_VERSION_2) {
//             /*    */ ChunkData_v2 chunks[nodeCount / ChunkData_v2.chunkNodeCount];
//         }
//     };
//
//     struct ThreadData { // offset 0x10
//         /*0x00*/ double secondsPerCycle;
//         /*0x08*/ FrameData frames[];
//     };
//
//     /*0x00*/ FileHeader fileHdr;
//     /*0x10*/ ThreadData data;
// };
//

#endif /* xxprofile_version_h */
