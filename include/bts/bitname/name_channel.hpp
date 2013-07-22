#pragma once
#include <bts/bitname/name_block.hpp>
#include <bts/network/server.hpp>
#include <fc/filesystem.hpp>

namespace bts { namespace bitname {

  namespace detail { class name_channel_impl; }

  class name_channel_delegate
  {
    public:
       virtual ~name_channel_delegate(){}

       /**
        *   Called any time a new & valid name reg trx is received
        */
       virtual void pending_name_registration( const name_trx&  ) = 0;

       /**
        *   Called any time a new & valid name block is added to the
        *   chain or replaces the head block.
        */
       virtual void name_block_added( const name_block& b ) = 0;
  };


  /**
   *  Subscribes to the name registration channel, downloads the
   *  database, and provides callbacks anytime something changes.
   */
  class name_channel 
  {
      public:
        struct config
        {
           fc::path name_db_dir;
        };

        name_channel( const bts::network::server_ptr& n );
        ~name_channel();

        void configure( const name_channel::config& c );
        void set_delegate( name_channel_delegate* d );

        void submit_name( const name_trx& t );
        void submit_block( const name_block& b );

        /**
         *  Performs a lookup in the internal database 
         */
        name_header lookup_name( const std::string& name );

      private:
        std::unique_ptr<detail::name_channel_impl> my;
  };

  /**
   *  Defines the message codes that are broadcast on the name_proto
   *  channel.
   */
  enum bitname_message_types 
  {
     name_inv_msg_id       = 1, ///< an array of name mini_pows
     block_inv_msg_id      = 2, ///< an array of name block pows
     name_trx_msg_id       = 3, ///< a single name reg trx
     name_block_msg_id     = 4, ///< a single block
     block_headers_msg_id  = 5, ///< a single block
     get_headers_msg_id    = 6, ///< request headers for a range of blocks
     get_trx_msg_id        = 7, ///< request a name trx by id, causess name_trx response
     get_block_msg_id      = 8  ///< request a name block by id, causess name_block response
  };

  struct name_trx_msg
  {
      enum type_enum { type = bitname_message_types::name_trx_msg_id };
      name_trx name;
  };

  struct block_msg
  {
      enum type_enum { type = bitname_message_types::name_block_msg_id };
      name_block block;
  };

  struct name_inv_msg
  {
      enum type_enum { type = bitname_message_types::name_inv_msg_id };
      std::vector<uint64_t>  names;
  };
  struct get_trx_msg
  {
      enum type_enum { type = bitname_message_types::get_trx_msg_id };
      std::vector<uint64_t>  names; 
  };

  struct get_headers_msg
  {
      enum type_enum { type = bitname_message_types::get_headers_msg_id };
      /**
       *  A list of known block headers starting with the most recent
       *  known block first, then working back further in the chain
       *  skipping more blocks.  This is designed to help detect a
       *  chain split.  The response will a set of headers branching
       *  from the most recent valid hash.
       */
      std::vector<mini_pow>  locator_hashes; 
  };

  struct get_block_msg 
  {
     enum type_enum { type = bitname_message_types::get_block_msg_id };
     mini_pow block_id;
  };

  struct block_headers_msg
  {
     enum type_enum { type = bitname_message_types::block_headers_msg_id };
     fc::unsigned_int             start_index; // 
     std::vector<name_header> headers;
  };


 } } // bts::bitname

 FC_REFLECT( bts::bitname::name_trx_msg, (name) )
 FC_REFLECT( bts::bitname::block_msg, (block) )
 FC_REFLECT( bts::bitname::name_inv_msg, (names) )
 FC_REFLECT( bts::bitname::get_trx_msg, (names) )
 FC_REFLECT( bts::bitname::get_headers_msg, (locator_hashes) )
 FC_REFLECT( bts::bitname::get_block_msg, (block_id) )
 FC_REFLECT( bts::bitname::block_headers_msg, (start_index)(headers) )
