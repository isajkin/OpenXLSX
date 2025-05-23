/*

   ____                               ____      ___ ____       ____  ____      ___
  6MMMMb                              `MM(      )M' `MM'      6MMMMb\`MM(      )M'
 8P    Y8                              `MM.     d'   MM      6M'    ` `MM.     d'
6M      Mb __ ____     ____  ___  __    `MM.   d'    MM      MM        `MM.   d'
MM      MM `M6MMMMb   6MMMMb `MM 6MMb    `MM. d'     MM      YM.        `MM. d'
MM      MM  MM'  `Mb 6M'  `Mb MMM9 `Mb    `MMd       MM       YMMMMb     `MMd
MM      MM  MM    MM MM    MM MM'   MM     dMM.      MM           `Mb     dMM.
MM      MM  MM    MM MMMMMMMM MM    MM    d'`MM.     MM            MM    d'`MM.
YM      M9  MM    MM MM       MM    MM   d'  `MM.    MM            MM   d'  `MM.
 8b    d8   MM.  ,M9 YM    d9 MM    MM  d'    `MM.   MM    / L    ,M9  d'    `MM.
  YMMMM9    MMYMMM9   YMMMM9 _MM_  _MM_M(_    _)MM_ _MMMMMMM MYMMMM9 _M(_    _)MM_
            MM
            MM
           _MM_

  Copyright (c) 2018, Kenneth Troldal Balslev

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  - Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  - Neither the name of the author nor the
    names of any contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#ifndef OPENXLSX_XLDOCUMENT_HPP
#define OPENXLSX_XLDOCUMENT_HPP

#ifdef _MSC_VER    // conditionally enable MSVC specific pragmas to avoid other compilers warning about unknown pragmas
#   pragma warning(push)
#   pragma warning(disable : 4251)
#   pragma warning(disable : 4275)
#endif // _MSC_VER

// ===== External Includes ===== //
#include <algorithm> // std::find_if
#include <list>
#include <string>

// ===== OpenXLSX Includes ===== //
#include "IZipArchive.hpp"
#include "OpenXLSX-Exports.hpp"
#include "XLCommandQuery.hpp"
#include "XLComments.hpp"
#include "XLContentTypes.hpp"
#include "XLDrawing.hpp"
#include "XLDrawing1.hpp"
#include "XLProperties.hpp"
#include "XLRelationships.hpp"
#include "XLSharedStrings.hpp"
#include "XLStyles.hpp"
#include "XLTables.hpp"
#include "XLWorkbook.hpp"
#include "XLXmlData.hpp"
#include "XLZipArchive.hpp"

namespace OpenXLSX
{
 
    constexpr const unsigned int pugi_parse_settings = pugi::parse_default | pugi::parse_ws_pcdata; // TBD: | pugi::parse_comments

    constexpr const bool XLForceOverwrite = true;    // readability constant for 2nd parameter of XLDocument::saveAs
    constexpr const bool XLDoNotOverwrite = false;   //  "

    /**
     * @brief The XLDocumentProperties class is an enumeration of the possible properties (metadata) that can be set
     * for a XLDocument object (and .xlsx file)
     */
    enum class XLProperty {
        Title,
        Subject,
        Creator,
        Keywords,
        Description,
        LastModifiedBy,
        LastPrinted,
        CreationDate,
        ModificationDate,
        Category,
        Application,
        DocSecurity,
        ScaleCrop,
        Manager,
        Company,
        LinksUpToDate,
        SharedDoc,
        HyperlinkBase,
        HyperlinksChanged,
        AppVersion
    };

    /**
     * @brief This class encapsulates the concept of an excel file. It is different from the XLWorkbook, in that an
     * XLDocument holds an XLWorkbook together with its metadata, as well as methods for opening,
     * closing and saving the document.\n<b><em>The XLDocument is the entrypoint for clients
     * using the RapidXLSX library.</em></b>
     */
    class OPENXLSX_EXPORT XLDocument final
    {
        //----- Friends
        friend class XLXmlFile;
        friend class XLWorkbook;
        friend class XLSheet;
        friend class XLXmlData;

        //---------- Public Member Functions
    public:
        /**
         * @brief Constructor. The default constructor with no arguments.
         */
        explicit XLDocument(const IZipArchive& zipArchive = XLZipArchive());

        /**
         * @brief Constructor. An alternative constructor, taking the path to the .xlsx file as an argument.
         * @param docPath A std::string with the path to the .xlsx file.
         * @param zipArchive
         */
        explicit XLDocument(const std::string& docPath, const IZipArchive& zipArchive = XLZipArchive());

        /**
         * @brief Copy constructor
         * @param other The object to copy
         * @note Copy constructor explicitly deleted.
         */
        XLDocument(const XLDocument& other) = delete;

        /**
         * @brief
         * @param other
         */
        XLDocument(XLDocument&& other) noexcept = default;

        /**
         * @brief Destructor
         */
        ~XLDocument();

        /**
         * @brief
         * @param other
         * @return
         */
        XLDocument& operator=(const XLDocument& other) = delete;

        /**
         * @brief
         * @param other
         * @return
         */
        XLDocument& operator=(XLDocument&& other) noexcept = default;

        /**
         * @brief ensure that warnings are shown (default setting)
         */
        void showWarnings();

        /**
         * @brief ensure that warnings are suppressed where this parameter is supported (currently only XLStyles)
         */
        void suppressWarnings();

        /**
         * @brief Open the .xlsx file with the given path
         * @param fileName The path of the .xlsx file to open
         */
        void open(const std::string& fileName);

        /**
         * @brief Create a new .xlsx file with the given name.
         * @param fileName The path of the new .xlsx file.
         * @param forceOverwrite If not true (XLForceOverwrite) and fileName exists, create will throw an exception
         * @throw XLException (OpenXLSX failed checks)
         * @throw ZipRuntimeError (zippy failed archive / file access)
         */
        void create(const std::string& fileName, bool forceOverwrite);

        /**
         * @brief Create a new .xlsx file with the given name. Legacy interface, invokes create( fileName, XLForceOverwrite )
         * @param fileName The path of the new .xlsx file.
         * @deprecated use instead void create(const std::string& fileName, bool forceOverwrite)
         * @warning Overwriting an existing file is retained as legacy behavior, but can lead to data loss!
         */
        [[deprecated]] void create(const std::string& fileName);

        /**
         * @brief Close the current document
         */
        void close();

        /**
         * @brief Save the current document using the current filename, overwriting the existing file.
         * @throw XLException (OpenXLSX failed checks)
         * @throw ZipRuntimeError (zippy failed archive / file access)
         */
        void save();

        /**
         * @brief Save the document with a new name. If a file exists with that name, it will be overwritten.
         * @param fileName The path of the file
         * @param forceOverwrite If not true (XLForceOverwrite) and fileName exists, saveAs will throw an exception
         * @throw XLException (OpenXLSX failed checks)
         * @throw ZipRuntimeError (zippy failed archive / file access)
         */
        void saveAs(const std::string& fileName, bool forceOverwrite);

        /**
         * @brief Save the document with a new name. Legacy interface, invokes saveAs( fileName, XLForceOverwrite )
         * @param fileName The path of the file
         * @deprecated use instead void saveAs(const std::string& fileName, bool forceOverwrite)
         * @warning Overwriting an existing file is retained as legacy behavior, but can lead to data loss!
         */
        [[deprecated]] void saveAs(const std::string& fileName);

        /**
         * @brief Get the filename of the current document, e.g. "spreadsheet.xlsx".
         * @return A std::string with the filename.
         * @note 2024-06-03: function can't return as reference to const because filename as a substr of m_filePath can be a temporary
         * @note 2024-07-28: Removed const from return type
         */
        std::string name() const;

        /**
         * @brief Get the full path of the current document, e.g. "drive/blah/spreadsheet.xlsx"
         * @return A std::string with the path.
         */
        const std::string& path() const;

        /**
         * @brief Get the underlying workbook object, as a const object.
         * @return A const pointer to the XLWorkbook object.
         */
        XLWorkbook workbook() const;

        /**
         * @brief Get the requested document property.
         * @param prop The name of the property to get.
         * @return The property as a string
         */
        std::string property(XLProperty prop) const;

        /**
         * @brief Set a property
         * @param prop The property to set.
         * @param value The getValue of the property, as a string
         */
        void setProperty(XLProperty prop, const std::string& value);

        /**
         * @brief Delete the property from the document
         * @param theProperty The property to delete from the document
         */
        void deleteProperty(XLProperty theProperty);

        /**
         * @brief
         * @return
         */
        explicit operator bool() const;

        /**
         * @brief
         * @return
         */
        bool isOpen() const;

        /**
         * @brief return a handle on the workbook's styles
         * @return a reference to m_styles
         */
        XLStyles& styles();

        /**
         * @brief determine whether a worksheet relationships file exists for sheetXmlNo
         * @param sheetXmlNo check for this sheet number # (xl/worksheets/_reals/sheet#.xml.rels)
         * @return true if relationships file exists
         */
        bool hasSheetRelationships(uint16_t sheetXmlNo) const;

        /**
         * @brief determine whether a worksheet vml drawing file exists for sheetXmlNo
         * @param sheetXmlNo check for this sheet number # (xl/drawings/vmlDrawing#.xml)
         * @return true if vml drawing file exists
         */
        bool hasSheetVmlDrawing(uint16_t sheetXmlNo) const;
        bool hasSheetDrawing1(uint16_t sheetXmlNo) const;

        /**
         * @brief determine whether a worksheet comments file exists for sheetXmlNo
         * @param sheetXmlNo check for this sheet number # (xl/comments#.xml)
         * @return true if comments file exists
         */
        bool hasSheetComments(uint16_t sheetXmlNo) const;

        /**
         * @brief determine whether a worksheet table(s) file exists for sheetXmlNo
         * @param sheetXmlNo check for this sheet number # (xl/tables/table#.xml)
         * @return true if table(s) file exists
         */
        bool hasSheetTables(uint16_t sheetXmlNo) const;

        /**
         * @brief fetch the worksheet relationships for sheetXmlNo, create the file if it does not exist
         * @param sheetXmlNo fetch for this sheet #
         * @return an XLRelationships object initialized with the sheet relationships
         */
        XLRelationships sheetRelationships(uint16_t sheetXmlNo);

        /**
         * @brief fetch the worksheet VML drawing for sheetXmlNo, create the file if it does not exist
         * @param sheetXmlNo fetch for this sheet #
         * @return an XLVmlDrawing object initialized with the sheet drawing
         */
        XLVmlDrawing sheetVmlDrawing(uint16_t sheetXmlNo);
        XLDrawing1 sheetDrawing1(uint16_t sheetXmlNo);

        /**
         * @brief fetch the worksheet comments for sheetXmlNo, create the file if it does not exist
         * @param sheetXmlNo fetch for this sheet #
         * @return an XLComments object initialized with the sheet comments
         */
        XLComments sheetComments(uint16_t sheetXmlNo);

        /**
         * @brief fetch the worksheet tables for sheetXmlNo, create the file if it does not exist
         * @param sheetXmlNo fetch for this sheet #
         * @return an XLTables object initialized with the sheet tables
         */
        XLTables sheetTables(uint16_t sheetXmlNo);

        /**
         * @brief
         * @param command
         * @return for XLCommandType::SetSheetActive: execution success, otherwise always true
         */
        bool execCommand(const XLCommand& command);

        /**
         * @brief
         * @param query
         * @return
         */
        XLQuery execQuery(const XLQuery& query) const;

        /**
         * @brief
         * @param query
         * @return
         */
        XLQuery execQuery(const XLQuery& query);

        /**
         * @brief configure an alternative XML saving declaration to be used with pugixml
         * @param savingDeclaration An XLXmlSavingDeclaration object with the configuration to use
         * @return
         */
        void setSavingDeclaration(XLXmlSavingDeclaration const& savingDeclaration);

        /**
         * @brief
         * @return
         */
        const XLSharedStrings& sharedStrings() const { return m_sharedStrings; }

        /**
         * @brief rewrite the shared strings cache (and update all cells referencing an index from the shared strings), dropping unused strings
         * @note potentially time-intensive (on documents with many strings or many cells referring shared strings)
         */
        void cleanupSharedStrings();

        //----------------------------------------------------------------------------------------------------------------------
        //           Protected Member Functions
        //----------------------------------------------------------------------------------------------------------------------

    protected:
        /**
         * @brief Get an XML file from the .xlsx archive.
         * @param path The relative path of the file.
         * @return A std::string with the content of the file
         */
        std::string extractXmlFromArchive(const std::string& path);

        /**
         * @brief fetch the XLXmlData object as stored in m_data, throw XLInternalError if path is not found
         * @param path The relative path of the file.
         * @param doNotThrow if true, will return a nullptr if path is not found
         * @return a pointer to the XLXmlData object stored in m_data (or nullptr, see doNotThrow)
         */
public:		 
       XLXmlData* getXmlData(const std::string& path, bool doNotThrow = false);

        /**
         * @brief const overload of getXmlData
         * @param path
         * @param doNotThrow
         * @return
         */
        const XLXmlData* getXmlData(const std::string& path, bool doNotThrow = false) const;

        /**
         * @brief
         * @param path
         * @return
         */
        bool hasXmlData(const std::string& path) const;

        //----------------------------------------------------------------------------------------------------------------------
        //           Private Member Variables
        //----------------------------------------------------------------------------------------------------------------------

    private:
        bool m_suppressWarnings {true}; /**< If true, will suppress output of warnings where supported */

        std::string m_filePath {};      /**< The path to the original file*/

        XLXmlSavingDeclaration m_xmlSavingDeclaration;  /**< The xml saving declaration that will be passed to pugixml before generating the XML output data*/

public:
        mutable std::list<XLXmlData>    m_data {};              /**<  */
private:
        mutable std::deque<std::string> m_sharedStringCache {}; /**<  */
        mutable XLSharedStrings         m_sharedStrings {};     /**<  */

        XLRelationships m_docRelationships {}; /**< A pointer to the document relationships object*/
        XLRelationships m_wbkRelationships {}; /**< A pointer to the document relationships object*/
        XLRelationships m_wrkRelationships {}; /**< A pointer to the document relationships object*/
public: 
       XLRelationships m_drwRelationships {}; /**< A pointer to the document relationships object*/
       XLContentTypes  m_contentTypes {};     /**< A pointer to the content types object*/
private:
        XLAppProperties m_appProperties {};    /**< A pointer to the App properties object */
        XLProperties    m_coreProperties {};   /**< A pointer to the Core properties object*/
        XLStyles        m_styles {};           /**< A pointer to the document styles object*/
        XLWorkbook      m_workbook {};         /**< A pointer to the workbook object */
public:
        IZipArchive     m_archive {};          /**<  */
    };


    //----------------------------------------------------------------------------------------------------------------------
    //           Global utility functions
    //----------------------------------------------------------------------------------------------------------------------

    /**
     * @brief Get a hexadecimal representation of size bytes, starting at data
     * @param data A pointer to the data bytes to format
     * @param size The amount of data bytes to format
     * @return A string with the base-16 representation of the data bytes
     * @note 2024-08-18 BUGFIX: replaced char array with std::string, as ISO C++ standard does not permit variable size arrays
     */
    OPENXLSX_EXPORT std::string BinaryAsHexString(const void *data, const size_t size);

    /**
     * @brief Calculate the two-byte XLSX password hash for password
     * @param password the string to hash
     * @return the two byte value calculated according to the XLSX password hashing algorithm
     */
    OPENXLSX_EXPORT uint16_t ExcelPasswordHash (std::string password);
    /**
     * @brief Same as ExcelPasswordHash but format the output as a 4-digit hexadecimal string
     * @param password the string to hash
     * @return a string that can be stored in OOXML as a password hash
     */
    OPENXLSX_EXPORT std::string ExcelPasswordHashAsString (std::string password);

    /**
     * @brief eliminate from pathA leading subdirectories shared with pathB and find a path from pathB to pathA destination
     * @param pathA return a relative path to here
     * @param pathB escape this path via "../" until the common branch with pathA is reached
     * @return a string that leads via a relative path from pathA to pathB
     * @throw XLInternalError if pathA and pathB have no common leading (sub)directory
     */
    std::string getPathARelativeToPathB(std::string const& pathA, std::string const& pathB);

    /**
     * @brief eliminate from path any . and .. subdirectories by ignoring them (.) or escaping to the parent directory (..)
     * @param path the path to normalize in this way
     * @return a normalized path (no longer contains . or .. entries)
     * @throw XLInternalError upon invalid path - e.g. containing "//" or trying to escape via ".." beyond the context of path
     */
    std::string eliminateDotAndDotDotFromPath(const std::string& path);

}    // namespace OpenXLSX

#ifdef _MSC_VER    // conditionally enable MSVC specific pragmas to avoid other compilers warning about unknown pragmas
#   pragma warning(pop)
#endif // _MSC_VER

#endif    // OPENXLSX_XLDOCUMENT_HPP
