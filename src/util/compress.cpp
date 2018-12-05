#include "Parameters.h"
#include "DBReader.h"
#include "DBWriter.h"
#include "Debug.h"

#ifdef OPENMP
#include <omp.h>
#endif

int doCompression(int argc, const char **argv, const Command& command, bool shouldCompress) {
    Parameters& par = Parameters::getInstance();
    par.parseParameters(argc, argv, command, 2);

    DBReader<unsigned int> reader(par.db1.c_str(), par.db1Index.c_str(), par.threads, DBReader<unsigned int>::USE_INDEX|DBReader<unsigned int>::USE_DATA);
    reader.open(DBReader<unsigned int>::NOSORT);
    if (shouldCompress == true && reader.isCompressed() == true) {
        Debug(Debug::INFO) << "Database is already compressed.\n";
        return EXIT_SUCCESS;
    }
    if (shouldCompress == false && reader.isCompressed() == false) {
        Debug(Debug::INFO) << "Database is already decompressed.\n";
        return EXIT_SUCCESS;
    }

    int dbtype = reader.getDbtype();
    dbtype = shouldCompress ? dbtype | (1 << 31) : dbtype & ~(1 << 31);
    DBWriter writer(par.db2.c_str(), par.db2Index.c_str(), par.threads, shouldCompress, dbtype);
    writer.open();

    Debug(Debug::INFO) << "Start writing to database " << par.db2 << "\n";
    unsigned int* seqLens = reader.getSeqLens();
#pragma omp parallel
    {
        unsigned int thread_idx = 0;
#ifdef OPENMP
        thread_idx = (unsigned int) omp_get_thread_num();
#endif

#pragma omp for schedule(dynamic, 1)
        for (size_t i = 0; i < reader.getSize(); ++i) {
            Debug::printProgress(i);
            writer.writeData(reader.getData(i, thread_idx), std::max(seqLens[i], 1u) - 1u, reader.getDbKey(i), thread_idx);
        }
    }
    writer.close();
    reader.close();

    return EXIT_SUCCESS;
}

int compress(int argc, const char **argv, const Command& command) {
    return doCompression(argc, argv, command, true);
}

int decompress(int argc, const char **argv, const Command& command) {
    return doCompression(argc, argv, command, false);
}